#ifndef ANIME_H
#define ANIME_H
#include <pch.h>

#define STATUS_NOT_STARTED "Not Started"
#define STATUS_WAITING "Waiting"
#define STATUS_PROCESSING "Processing"
#define STATUS_FAILED "Failed"
#define STATUS_FINISHED "Finished"

namespace Upscaler {
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
        std::string Status;
    };
}

#endif //ANIME_H
