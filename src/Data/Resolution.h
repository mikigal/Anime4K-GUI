#ifndef RESOLUTION_H
#define RESOLUTION_H
#include <format>
#include <pch.h>
#include <utility>

namespace Upscaler {
    class Resolution {
    public:
        int Width;
        int Height;
        std::string AspectRatio;
        std::string VisibleName;

        Resolution(int width, int height, std::string aspectRatio)
            : Width(width), Height(height), AspectRatio(std::move(aspectRatio)),
              VisibleName(std::format("{}x{} ({})", width, height, AspectRatio)) {}
    };
}

#endif //RESOLUTION_H
