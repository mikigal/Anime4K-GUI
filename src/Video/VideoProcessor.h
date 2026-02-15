#ifndef ANIME4K_GUI_VIDEOPROCESSOR_H
#define ANIME4K_GUI_VIDEOPROCESSOR_H

namespace Upscaler {
    class Video;
    class Shader;
    class Resolution;
    class Encoder;
    class App;

    class VideoProcessor {
    public:
        App* Instance;
        std::thread Worker;
        std::atomic<bool> CancelRequested;

        std::string GetStatusFromLine(std::string& line, const std::string& field);
        void ValidateFFmpeg();
        void HandleButton();
        void StartBatchProcessing();
        void StartVideoProcessing(Encoder& encoder, Resolution& resolution, Shader& shader, Video& video, std::string& outputFormat);
        void CancelProcessing();
        std::string BuildFFmpegCommand(Encoder& encoder, Resolution& resolution, Shader& shader, Video& video, std::string& outputFormat);
        bool IsProcessing();

        VideoProcessor(App* instance)
            : Instance(instance) {
        }
    };
}

#endif //ANIME4K_GUI_VIDEOPROCESSOR_H