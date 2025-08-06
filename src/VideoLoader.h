#ifndef VIDEOLOADER_H
#define VIDEOLOADER_H
#include "Data/Video.h"
#include "App.h"

namespace Upscaler {
    class VideoLoader {
    public:
        App& m_App;
        std::vector<Video> m_Videos;
        void loadVideo(std::string& path);

        VideoLoader(App& m_app)
            : m_App(m_app) {
        }
    };
}

#endif //VIDEOLOADER_H
