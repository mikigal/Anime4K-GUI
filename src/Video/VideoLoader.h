#ifndef VIDEOLOADER_H
#define VIDEOLOADER_H
#include "Data/Video.h"

namespace Upscaler {
    class App;

    class VideoLoader {
    private:
        App* Instance;
        std::vector<Video> m_Videos;

    public:
        void LoadVideo(std::string& path);

        VideoLoader(App* instance)
            : Instance(instance) {
        }

        [[nodiscard]] std::vector<Video>& GetVideos() {
            return m_Videos;
        }
    };
}

#endif //VIDEOLOADER_H
