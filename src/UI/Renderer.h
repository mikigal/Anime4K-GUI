#ifndef RENDERER_H
#define RENDERER_H

namespace Upscaler {
    class App;

    class Renderer {
    public:
        ImFont* m_Font;
        App* Instance;

        // UI pointers
        int SelectedResolution = 0;
        int SelectedShader = 0;
        int SelectedEncoder = 0;
        int SelectedCrf = 0;
        int SelectedCq = 0;
        int SelectedCpuThreads = 0;
        bool SelectedDebugMode = false;

        static std::string Logs;
        float Progress = 0;
        float Speed = 0;
        int EtaSeconds = 0;

        bool Init();
        void Terminate();
        void RenderUI();

        Renderer(App* instance)
            : Instance(instance) {
        }

    private:
        GLFWwindow* m_Window = nullptr;

        void InitializeWindow();
        void ApplyStyle();
    };
} // Upscaler

#endif //RENDERER_H
