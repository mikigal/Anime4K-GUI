#ifndef RESOLUTION_H
#define RESOLUTION_H
#include <format>
#include <pch.h>
#include <utility>

namespace Upscaler {
    class Resolution {
    private:
        int m_Width;
        int m_Height;
        std::string m_AspectRatio;
        std::string m_VisibleName;
        bool m_Custom;

    public:
        Resolution(int width, int height, std::string aspectRatio)
            : m_Width(width), m_Height(height), m_AspectRatio(std::move(aspectRatio)) {
            m_VisibleName = m_Width == 0 ? "Custom" : std::format("{}x{} ({})", width, height, m_AspectRatio);
            m_Custom = m_Width == 0;
        }

        [[nodiscard]] int GetWidth() const {
            return m_Width;
        }

        [[nodiscard]] int GetHeight() const {
            return m_Height;
        }

        [[nodiscard]] std::string GetAspectRatio() const{
            return m_AspectRatio;
        }

        [[nodiscard]] const std::string& GetVisibleName() const {
            return m_VisibleName;
        }

        [[nodiscard]] bool IsCustom() const {
            return m_Custom;
        }
    };
}

#endif //RESOLUTION_H
