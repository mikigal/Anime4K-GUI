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
        TinyProcessLib::Process* Process = nullptr;
        bool Processing = false;

        std::string GetStatusFromLine(std::string& line, const std::string& field);
        void ValidateFFmpeg();
        void HandleButton();
        void StartBatchProcessing();
        void StartVideoProcessing(Encoder& encoder, Resolution& resolution, Shader& shader, Video& video, std::string& outputFormat);
        void CancelProcessing();
        std::string BuildFFmpegCommand(Encoder& encoder, Resolution& resolution, Shader& shader, Video& video, std::string& outputFormat);

        VideoProcessor(App* instance)
            : Instance(instance) {
        }
    };
}

#endif //ANIME4K_GUI_VIDEOPROCESSOR_H