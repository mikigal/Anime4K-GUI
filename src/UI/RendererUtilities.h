#ifndef ANIME4K_GUI_RENDERERUTILITIES_H
#define ANIME4K_GUI_RENDERERUTILITIES_H
#include "imgui.h"
#include "pch.h"

namespace Upscaler {
    class RendererUtilities {
    public:
        static void CenteredText(const char* fmt, ...) {
            va_list args;
            va_start(args, fmt);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() +
                                 (ImGui::GetTextLineHeightWithSpacing() - ImGui::GetTextLineHeight()) * 0.5f);
            ImGui::TextV(fmt, args);
            va_end(args);
        }

        static void ComboWithLabel(const char* label, const char* tooltip, const char* id, int* current,
                                      const std::vector<const char*>& items) {
            ImGui::Text("%s", label);
            if (ImGui::IsItemHovered() && tooltip != nullptr) ImGui::SetTooltip("%s", tooltip);
            ImGui::SetNextItemWidth(300);
            ImGui::Combo(id, current, items.data(), items.size());
            if (ImGui::IsItemHovered() && tooltip != nullptr) ImGui::SetTooltip("%s", tooltip);
            ImGui::Spacing();
        }

        static void NumberInput(const char* label, const char* tooltip, const char* id, int* current) {
            ImGui::Text("%s", label);
            ImGui::SetNextItemWidth(300);
            ImGui::InputInt(id, current);
            if (ImGui::IsItemHovered() && tooltip != nullptr) ImGui::SetTooltip("%s", tooltip);
            ImGui::Spacing();
        }
    };
}

#endif //ANIME4K_GUI_RENDERERUTILITIES_H