#ifndef RESOLUTION_H
#define RESOLUTION_H
#include <pch.h>

namespace Upscaler {
    class Resolution {
    public:
        int m_Width;
        int m_Height;
        std::string m_AspectRatio;

        Resolution(int m_width, int m_height, const std::string &m_aspect_ratio)
            : m_Width(m_width),
              m_Height(m_height),
              m_AspectRatio(m_aspect_ratio) {
        }
    };
}

#endif //RESOLUTION_H
