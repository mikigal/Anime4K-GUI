#ifndef RENDERER_H
#define RENDERER_H
#include "App.h"

namespace Upscaler {

class Renderer {
public:
    ImFont* m_Font;
    bool Init();
    void Terminate();
    void RenderUI();
    App& m_App;

    Renderer(App &m_app)
        : m_App(m_app) {
    }
private:
    GLFWwindow* m_Window = nullptr;

    void InitializeWindow();
    void ApplyStyle();
};

} // Upscaler

#endif //RENDERER_H
