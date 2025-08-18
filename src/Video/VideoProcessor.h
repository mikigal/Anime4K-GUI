#ifndef ANIME4K_GUI_VIDEOPROCESSOR_H
#define ANIME4K_GUI_VIDEOPROCESSOR_H

namespace Upscaler {
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

        VideoProcessor(App* instance)
            : Instance(instance) {
        }
    };
}

#endif //ANIME4K_GUI_VIDEOPROCESSOR_H