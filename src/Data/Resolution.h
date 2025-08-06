#ifndef RESOLUTION_H
#define RESOLUTION_H
#include <format>
#include <pch.h>

namespace Upscaler {
    struct Resolution {
        const int Width;
        const int Height;
        const std::string AspectRatio;
        const std::string VisibleName = std::format("{}x{} ({})", Width, Height, AspectRatio);
    };
}

#endif //RESOLUTION_H
