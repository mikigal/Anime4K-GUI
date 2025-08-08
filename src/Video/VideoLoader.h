#ifndef VIDEOLOADER_H
#define VIDEOLOADER_H
#include "Data/Video.h"

namespace Upscaler {
    class App;

    class VideoLoader {
    public:
        App* Instance;
        std::vector<Video> m_Videos;

        void LoadVideo(std::string& path);

        VideoLoader(App* instance)
            : Instance(instance) {
        }
    };
}

#endif //VIDEOLOADER_H
