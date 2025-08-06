#ifndef RESOLUTION_H
#define RESOLUTION_H
#include <pch.h>

namespace Upscaler {
    struct Resolution {
        const int Width;
        const int Height;
        const std::string AspectRatio;
    };
}

#endif //RESOLUTION_H
