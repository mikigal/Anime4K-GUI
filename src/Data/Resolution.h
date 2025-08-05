#ifndef RESOLUTION_H
#define RESOLUTION_H

namespace Upscaler {
    class Shader {
    public:
        int m_Width;
        int m_Height;
        bool m_Panoramic;

        Shader(int m_width, int m_height, bool m_panoramic)
            : m_Width(m_width),
              m_Height(m_height),
              m_Panoramic(m_panoramic) {
        }
    };
}

#endif //RESOLUTION_H
