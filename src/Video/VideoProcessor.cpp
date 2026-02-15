#include "VideoProcessor.h"

#include "App.h"
#include "pch.h"
#include "Utilities/Utilities.h"
#include <chrono>
#include <thread>

namespace Upscaler {

    void VideoProcessor::StartBatchProcessing() {
        std::vector<Video>& videos = Instance->GetVideoLoader().m_Videos;
        for (Video& video : videos) {
            std::cout << video.Name << std::endl;
            std::cout << video.Status << std::endl;
        }

        if (videos.empty() || std::ranges::all_of(videos,
            [](const Video& v){ return v.Status == STATUS_FINISHED; })) {
            Instance->GetLogger().Error("There's no videos to be upscaled");
            return;
        }

        Encoder& encoder = Instance->GetRenderer().GetSelectedEncoder();
        Resolution& resolution = Instance->GetRenderer().GetSelectedResolution();
        Shader& shader = Instance->GetRenderer().GetSelectedShader();
        std::string& outputFormat = Instance->GetRenderer().GetSelectedOutputFormat();

        Instance->GetLogger().Debug("Selected encoder: {}", encoder.Name);
        Instance->GetLogger().Debug("Selected resolution: {}", resolution.VisibleName);
        Instance->GetLogger().Debug("Selected shader: {}", shader.Name);

        Processing = true;
        for (Video& video : videos) {
            if (video.Status != STATUS_FINISHED) {
                video.Status = STATUS_WAITING;
            }
        }

        for (int i = 0; i < videos.size(); i++) {
            if (videos[i].Status == STATUS_FINISHED) {
                continue;
            }

            Video& video = videos[i];

            Instance->GetLogger().Info("Processing video: {} ({} / {})", video.Name, i + 1, videos.size());
            StartVideoProcessing(encoder, resolution, shader, video, outputFormat);
        }

        Processing = false;

    }

    void VideoProcessor::StartVideoProcessing(Encoder& encoder, Resolution& resolution, Shader& shader, Video& video, std::string& outputFormat) {
        std::string command = BuildFFmpegCommand(encoder, resolution, shader, video, outputFormat);
        Instance->GetLogger().Info("Command: {}", command);

        video.Status = STATUS_PROCESSING;
        video.Progress = 0;

        std::thread([&video, command, this]() {

            TinyProcessLib::Process process(command, "",
            [](const char* bytes, size_t n) {
                // For some reason ffmpeg outputs everything into stderr
            },
            [this, &video](const char* bytes, size_t n) {
                std::string line = std::string(bytes, n);

                if (!line.starts_with("frame=")) {
                    Instance->GetRenderer().Logs += line + "\n";
                    return;
                }

                std::string frameStr = GetStatusFromLine(line, "frame");
                std::string speedStr = GetStatusFromLine(line, "speed");

                try {
                    int frame = std::stoi(frameStr);
                    video.Progress = (frame / static_cast<float>(video.TotalFrames));
                    video.Speed = std::stof(Utilities::ReplaceAll(speedStr, "x", ""));
                    video.Eta = ((video.TotalFrames - frame) / video.FrameRate) / video.Speed;
                } catch (const std::exception& e) {
                    // FFMPEG sometimes don't show all values, so we can just ignore parsing errors
                }
            });

            int exitCode = process.get_exit_status();
            Instance->GetLogger().Debug("ffmpeg exited with code {}", exitCode);

            if (exitCode != 0) {
                Instance->GetLogger().Error("An error occurred while executing ffmpeg, exit code: {}", exitCode);
                return;
            }

            // Update UI
            video.Status = STATUS_FINISHED;
            video.Eta = -1;
            video.Speed = -1;
            video.Progress = 1;
        }).detach();
    }

    std::string VideoProcessor::BuildFFmpegCommand(Encoder& encoder, Resolution& resolution, Shader& shader, Video& video, std::string& outputFormat) {
        std::string command = "ffmpeg.exe "; // FFMPEG exec path
        command += "-hide_banner "; // Hide FFMPEG's banner
        command += "-y "; // Override output file
        command += std::format("-i \"{}\" ", video.Path); // Path to input video
        command += "-init_hw_device vulkan "; // Use Vulkan
        command += std::format("-vf libplacebo=w={}:h={}:upscaler=ewa_lanczos:custom_shader_path={},format={} ", // libplacebo filter setup
            resolution.Width, resolution.Height, shader.Path, video.PixelFormat);
        command += "-dn "; // Remove data streams


        command += outputFormat == "mkv" ? "-c:s copy " : "-sn "; // If output container is MKV copy subtitles steam, otherwise remove it
        command += "-c:a copy "; // Copy audio stream
        command += "-map 0:v? "; // Map all video streams if exists
        command += "-map 0:a? "; // Map all audio streams if exists
        command += "-map 0:s? "; // Map all subtitlies streams if exists
        command += "-map_metadata -1 "; // Do not copy metadata
        command += "-map_chapters -1 "; // Do not copy chapter information
        command += std::format("-c:v {} ", encoder.Value); // Set video encoder

        // Apply encoder specific parameters
        // If encoder does not support some placeholder it will be ignored
        for (const std::string& parameter : encoder.Params) {
            std::string replaced = Utilities::ReplaceAll(parameter, "{CRF}", Instance->GetRenderer().SelectedCrf);
            replaced = Utilities::ReplaceAll(replaced, "{CQ}", Instance->GetRenderer().SelectedCq);
            replaced = Utilities::ReplaceAll(replaced, "{VIDEOTOOLBOX_CQ}", Instance->GetRenderer().SelectedCq);
            command += replaced + " ";
        }

        // Apply encoder specific parameters
        // If encoder does not support thread limiting it will be ignored
        for (const std::string& parameter : encoder.ThreadsLimitParams) {
           command += Utilities::ReplaceAll(parameter, "{THREADS}", Instance->GetRenderer().SelectedCpuThreads) + " ";
        }

        // Add output file path
        command += std::format("\"{}\"", Utilities::AddUpscaledSuffix(video.Path));

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

    }

    void VideoProcessor::HandleButton() {
        StartBatchProcessing();
    }

    void VideoProcessor::ValidateFFmpeg() {
        std::string output;

        TinyProcessLib::Process process(
            "ffmpeg -version", "",
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
                Instance->GetLogger().Critical("Please install FFMPEG using brew package manager and make sure it's available in $PATH\n");
#endif
                Instance->GetRenderer().CriticalError = true;
            });

        int exitCode = process.get_exit_status();
        Instance->GetLogger().Debug("ffmpeg -version exited with code {}", exitCode);
        if (exitCode != 0) {
            return;
        }

        std::string version = Utilities::Split(output, "Copyright")[0];
        Instance->GetLogger().Info("{}\n", version);
        if (output.find("enable-libplacebo") == std::string::npos) {
            Instance->GetLogger().Critical("Your FFMPEG build does not include libplacebo, application will NOT work");
            Instance->GetLogger().Critical("Details about your FFMPEG: \n{}", output);
            Instance->GetRenderer().CriticalError = true;
        }
    }

}
