#include "VideoLoader.h"
#include <pch.h>

#include "Utilities/Logger.h"
#include "Utilities/Utilities.h"

namespace Upscaler {
    void VideoLoader::loadVideo(std::string& path) {
        LOG_INFO("Loading video metadata: {}", path);

        long startMillis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        std::filesystem::path filePath = std::filesystem::path("/Users/mikigal/Desktop/Another01.mp4");
        std::string jsonString = "";

        TinyProcessLib::Process process(
            "ffprobe -v quiet -print_format json -show_format -show_streams " + filePath.string(), "",
            [&jsonString](const char *bytes, size_t n) {
                jsonString.append(bytes, n);
            },
            [](const char *bytes, size_t n) {
                LOG_ERROR("An error occurred while executing ffprobe, stderr: {}", std::string(bytes, n));
            });

        int exitCode = process.get_exit_status();
        LOG_DEBUG("ffprobe exited with code {}", exitCode);
        if (exitCode != 0) {
            LOG_ERROR("An error occurred while executing ffprobe");
            return;
        }

        long size = std::filesystem::file_size(filePath);

        int height;
        int width;
        int duration;
        int frameRate;
        int totalFrames;
        std::string pixelFormat;
        std::vector<std::string> streamCodecs;

        nlohmann::json json = nlohmann::json::parse(jsonString);
        for (nlohmann::basic_json<> stream: json["streams"]) {
            if (stream["codec_type"] == "video") {
                height = stream["height"].get<int>();
                width = stream["width"].get<int>();
                duration = std::stof(stream["duration"].get<std::string>());
                pixelFormat = stream["pix_fmt"].get<std::string>();

                std::vector<std::string> frameRateSplit = split(stream["avg_frame_rate"].get<std::string>(), '/');
                float base = std::stof(frameRateSplit[0]);
                float divider = std::stof(frameRateSplit[1]);
                frameRate = base / divider;
            }

            streamCodecs.push_back(stream["codec_name"].get<std::string>());
        }

        totalFrames = frameRate * duration;
        Video video(filePath.filename(), duration, size, width, height, frameRate, totalFrames, false,
                              streamCodecs, pixelFormat, filePath.string(), ProcessingStatus::WAITING);

        long endMillis = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        LOG_INFO("Loaded video {} metadata, took {}ms", filePath.string(), endMillis - startMillis);

        m_Videos.push_back(video);
    }
}
