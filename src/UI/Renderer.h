
#ifndef RENDERER_H
#define RENDERER_H

namespace Upscaler {

class Renderer {
public:
    ImFont* m_Font;
    bool Initialize();
    void Terminate();
    void RenderUI();
private:
    GLFWwindow* m_Window = nullptr;
    void InitializeWindow();
    void ApplyStyle();
};

} // Upscaler

#endif //RENDERER_H
