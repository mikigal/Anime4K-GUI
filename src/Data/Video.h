#ifndef ANIME_H
#define ANIME_H
#include <pch.h>

namespace Upscaler {
    enum class ProcessingStatus {
        WAITING,
        PROCESSING,
        FINISHED,
        ERROR,
        NOT_STARTED
    };

    struct Video {
        std::string Name;
        int Duration;
        long Size;
        int Width;
        int Height;
        int FrameRate;
        int TotalFrames;
        bool HasSubtitlesStream;
        std::vector<std::string> StreamCodecs;
        std::string PixelFormat;
        std::string Path;
        ProcessingStatus Status;
    };


}

#endif //ANIME_H
