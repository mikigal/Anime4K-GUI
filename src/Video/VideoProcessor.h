#ifndef ANIME4K_GUI_VIDEOPROCESSOR_H
#define ANIME4K_GUI_VIDEOPROCESSOR_H

namespace Upscaler {
    class Video;
    class Shader;
    class Resolution;
    class Encoder;
    class App;

    class VideoProcessor {
    private:
        App* Instance;
        std::atomic<bool> m_CancelRequested;
        std::string GetStatusFromLine(std::string& line, const std::string& field);
        std::string BuildFFmpegCommand(Encoder& encoder, Resolution& resolution, Shader& shader, Video& video, std::string& outputFormat);
        void StartVideoProcessing(Encoder& encoder, Resolution& resolution, Shader& shader, Video& video, std::string& outputFormat);

    public:
        void ValidateFFmpeg();
        void HandleButton();
        void StartBatchProcessing();
        bool IsProcessing();
        void CancelProcessing();

        VideoProcessor(App* instance)
            : Instance(instance) {
        }
    };
}

#endif //ANIME4K_GUI_VIDEOPROCESSOR_H