#ifndef ANIME4K_GUI_VIDEOPROCESSOR_H
#define ANIME4K_GUI_VIDEOPROCESSOR_H

namespace Upscaler {
    struct Video;
    struct Shader;
    struct Resolution;
    struct Encoder;
    class App;

    class VideoProcessor {
    public:
        App* Instance;
        TinyProcessLib::Process* Process = nullptr;
        bool Processing = false;

        void ValidateFFmpeg();
        void HandleButton();
        void StartProcessing();
        void CancelProcessing();
        void StartFFmpegProcess();
        std::string BuildFFmpegCommand(Encoder& encoder, Resolution& resolution, Shader& shader, Video& video, std::string& outputFormat);

        VideoProcessor(App* instance)
            : Instance(instance) {
        }
    };
}

#endif //ANIME4K_GUI_VIDEOPROCESSOR_H