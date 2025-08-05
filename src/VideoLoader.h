#ifndef VIDEOLOADER_H
#define VIDEOLOADER_H
#include "Data/Video.h"

namespace Upscaler {
    class VideoLoader {
        public:
        std::vector<Video> m_Videos;
        void loadVideo(std::string& path);
    };
}

#endif //VIDEOLOADER_H
