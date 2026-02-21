#include "VideoProcessor.h"

#include "App.h"
#include "pch.h"
#include "Utilities/Utilities.h"
#include <chrono>
#include <thread>

namespace Upscaler {

    void VideoProcessor::StartBatchProcessing() {
        std::vector<Video>& videos = Instance->GetVideoLoader().GetVideos();

        if (videos.empty() || std::ranges::all_of(videos,
            [](Video& v){ return v.GetStatus() == STATUS_FINISHED; })) {
            Instance->GetLogger().Error("There's no videos to be upscaled");
            return;
        }

        Encoder& encoder = Instance->GetConfiguration().GetSelectedEncoder();
        Resolution& resolution = Instance->GetConfiguration().GetSelectedResolution();
        Shader& shader = Instance->GetConfiguration().GetSelectedShader();
        std::string& outputFormat = Instance->GetConfiguration().GetSelectedOutputFormat();

        Instance->GetLogger().Debug("Selected encoder: {}", encoder.GetName());
        Instance->GetLogger().Debug("Selected resolution: {}", resolution.GetVisibleName());
        Instance->GetLogger().Debug("Selected shader: {}", shader.GetName());

        for (Video& video : videos) {
            if (video.GetStatus() != STATUS_FINISHED) {
                video.SetStatus(STATUS_WAITING);
            }
        }

        std::thread([&videos, &encoder, &resolution, &shader, &outputFormat, this]() {
            for (int i = 0; i < videos.size(); i++) {

                // User cancelled processing, ffmpeg process was already killed so stop this thread
                if (this->m_CancelRequested) {
                    m_CancelRequested = false;
                    return;
                }

                if (videos[i].GetStatus() == STATUS_FINISHED) {
                    continue;
                }

                Video& video = videos[i];

                Instance->GetLogger().Info("Processing video: {} ({} / {})", video.GetName(), i + 1, videos.size());
                StartVideoProcessing(encoder, resolution, shader, video, outputFormat);
            }
        }).detach();

    }

    // It's already called in new thread from VideoProcessor::StartBatchProcessing()
    void VideoProcessor::StartVideoProcessing(Encoder& encoder, Resolution& resolution, Shader& shader, Video& video, std::string& outputFormat) {
        std::string command = BuildFFmpegCommand(encoder, resolution, shader, video, outputFormat);
        Instance->GetLogger().Debug("Command: {}", command);

        video.SetStatus(STATUS_PROCESSING);
        video.SetProgress(0);

        TinyProcessLib::Process* process = new TinyProcessLib::Process(command, "",
        [](const char* bytes, size_t n) {
            // For some reason ffmpeg outputs everything into stderr
        },
        [this, &video](const char* bytes, size_t n) {
            std::string line = std::string(bytes, n);

            // Print details about file in logs
            if (!line.starts_with("frame=")) {
                Instance->GetLogger().Debug("FFMPEG logs: {}", line);
                return;
            }

            // Extract progress information from ffmpeg's output
            std::string frameStr = GetStatusFromLine(line, "frame");
            std::string speedStr = GetStatusFromLine(line, "speed");

            // Update UI
            try {
                int frame = std::stoi(frameStr);
                video.SetProgress(frame / static_cast<float>(video.GetTotalFrames()));
                video.SetSpeed(std::stof(Utilities::ReplaceAll(speedStr, "x", "")));
                video.SetEta(((video.GetTotalFrames() - frame) / video.GetFrameRate()) / video.GetSpeed());
            } catch (const std::exception& e) {
                // FFMPEG sometimes don't show all values, so we can just ignore parsing errors
            }
        });

        video.SetUpscalingProcess(process);

        // Wait for the process to finish
        int exitCode = process->get_exit_status();
        delete process;
        video.SetUpscalingProcess(nullptr);
        Instance->GetLogger().Debug("ffmpeg exited with code {}", exitCode);

        // Process exited with error and user didn't cancel processing (then exitcode == 2)
        if (exitCode != 0 && !m_CancelRequested) {
            Instance->GetLogger().Error("An error occurred while executing ffmpeg, exit code: {}", exitCode);
            video.SetStatus(STATUS_FAILED);
            video.SetProgress(0);
            video.SetEta(-1);
            video.SetSpeed(-1);
            return;
        }

        // Update UI
        video.SetStatus(m_CancelRequested ? STATUS_CANCELLED : STATUS_FINISHED);
        video.SetProgress(m_CancelRequested ? 0 : 1);
        video.SetEta(-1);
        video.SetSpeed(-1);
    }

    std::string VideoProcessor::BuildFFmpegCommand(Encoder& encoder, Resolution& resolution, Shader& shader, Video& video, std::string& outputFormat) {
        std::string command = std::format("{} ", Utilities::GetFFmpegPath()); // FFMPEG exec path
        command += "-hide_banner "; // Hide FFMPEG's banner
        command += "-y "; // Override output file
        command += std::format("-i \"{}\" ", video.GetPath()); // Path to input video

        // Use Vulkan only on Windows and Linux
#ifndef __APPLE__
        command += "-init_hw_device vulkan ";
#endif

        command += std::format("-vf libplacebo=w={}:h={}:upscaler=ewa_lanczos:custom_shader_path={},format={} ", // libplacebo filter setup
            resolution.GetWidth(), resolution.GetHeight(), shader.GetPath(), video.GetPixelFormat());
        command += "-dn "; // Remove data streams


        command += outputFormat == "mkv" ? "-c:s copy " : "-sn "; // If output container is MKV copy subtitles steam, otherwise remove it
        command += "-c:a copy "; // Copy audio stream
        command += "-map 0:v? "; // Map all video streams if exists
        command += "-map 0:a? "; // Map all audio streams if exists
        command += "-map 0:s? "; // Map all subtitlies streams if exists
        command += "-map_metadata -1 "; // Do not copy metadata
        command += "-map_chapters -1 "; // Do not copy chapter information
        command += std::format("-c:v {} ", encoder.GetValue()); // Set video encoder

        // Apply encoder specific parameters
        // If encoder does not support some placeholder it will be ignored
        for (const std::string& parameter : encoder.GetParams()) {
            std::string replaced = Utilities::ReplaceAll(parameter, "{CRF}", Instance->GetConfiguration().m_Crf);
            replaced = Utilities::ReplaceAll(replaced, "{CQ}", Instance->GetConfiguration().m_Cq);
            replaced = Utilities::ReplaceAll(replaced, "{VIDEOTOOLBOX_CQ}", Instance->GetConfiguration().m_Cq);
            command += replaced + " ";
        }

        // Apply encoder specific parameters
        // If encoder does not support thread limiting it will be ignored
        for (const std::string& parameter : encoder.GetThreadsLimitParams()) {
           command += Utilities::ReplaceAll(parameter, "{THREADS}", Instance->GetConfiguration().m_CpuThreads) + " ";
        }

        // Add output file path
        command += std::format("\"{}\"", Utilities::AddUpscaledSuffix(video.GetPath()));

        return command;
    }

    std::string VideoProcessor::GetStatusFromLine(std::string& line, const std::string& field) {
        const std::string token = field + "=";
        const size_t pos = line.find(token);

        if (pos == std::string::npos)
            return {};

        size_t i = pos + token.size();

        while (i < line.size() && std::isspace(static_cast<unsigned char>(line[i])))
            ++i;

        size_t j = i;

        while (j < line.size() && !std::isspace(static_cast<unsigned char>(line[j])))
            ++j;

        return line.substr(i, j - i);
    }

    void VideoProcessor::CancelProcessing() {
        if (std::any_of(Instance->GetVideoLoader().GetVideos().begin(),
                        Instance->GetVideoLoader().GetVideos().end(),
                        [](Video& video) {
                            return video.GetStatus() == STATUS_PROCESSING;
                        }))
        {
            Instance->GetLogger().Info("Cancelling processing...");
            m_CancelRequested = true;
        }

        for (Video& video : Instance->GetVideoLoader().GetVideos()) {
            if (video.GetStatus() != STATUS_PROCESSING) {
                continue;
            }

            // UI is updated and pointer is deleted in VideoProcessor::StartVideoProcessing after process is killed
            video.GetUpscalingProcess()->kill();
        }
    }

    void VideoProcessor::HandleButton() {
        if (!IsProcessing()) {
            StartBatchProcessing();
            return;
        }

        CancelProcessing();
    }

    bool VideoProcessor::IsProcessing() {
        for (Video& video : Instance->GetVideoLoader().GetVideos()) {
            if (video.GetStatus() == STATUS_PROCESSING) {
                return true;
            }
        }

        return false;
    }

    void VideoProcessor::ValidateFFmpeg() {
        std::string output;

        TinyProcessLib::Process process(
            std::format("{} -version", Utilities::GetFFmpegPath()), "",
            [&output](const char* bytes, size_t n) {
                output.append(bytes, n);
            },
            [this](const char* bytes, size_t n) {
                std::string errorMessage = std::string(bytes, n);
                while (!errorMessage.empty() && (errorMessage.back() == '\n' || errorMessage.back() == '\r')) {
                    errorMessage.pop_back();
                }

                Instance->GetLogger().Critical("An error occurred while executing: ffmpeg -version, stderr: {}", errorMessage);
                Instance->GetLogger().Critical("There's problem with your FFMPEG installation, probably it wasn't found");
#ifdef _WIN32
                this->Instance->GetLogger().Critical("Make sure bundled FFMPEG is in the same directory as Anime4K-GUI.exe\n");
#elif __linux__
                Instance->GetLogger().Critical("Please install FFMPEG using your package manager and make sure it's available in $PATH\n");
#elif __APPLE__
                Instance->GetLogger().Critical("Could not find FFMPEG bundled in .app\n");
#endif
                Instance->SetCriticalError(true);
            });

        int exitCode = process.get_exit_status();
        Instance->GetLogger().Debug("ffmpeg -version exited with code {}", exitCode);
        if (exitCode != 0) {
            return;
        }

        std::string version = Utilities::Split(output, "Copyright")[0];
        Instance->GetLogger().Debug("{}\n", version);
        if (output.find("enable-libplacebo") == std::string::npos) {
            Instance->GetLogger().Critical("Your FFMPEG build does not include libplacebo, application will NOT work");
            Instance->GetLogger().Critical("Details about your FFMPEG: \n{}", output);
            Instance->SetCriticalError(true);
        }
    }

}
