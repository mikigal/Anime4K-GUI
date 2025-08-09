#ifndef ANIME4K_GUI_VIDEOPROCESSOR_H
#define ANIME4K_GUI_VIDEOPROCESSOR_H

namespace Upscaler {
    class App;

    class VideoProcessor {
    public:
        App* Instance;

        void ValidateFFmpeg();

        VideoProcessor(App* instance)
            : Instance(instance) {
        }
    };
}

#endif //ANIME4K_GUI_VIDEOPROCESSOR_H