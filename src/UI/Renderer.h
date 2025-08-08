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
        int SelectedOutputFormat = 0;
        bool SelectedDebugMode = false;

        static std::string Logs;
        float Progress = 0;
        float Speed = 0;
        int EtaSeconds = 0;

        bool Init();
        void Terminate();

        Renderer(App* instance)
            : Instance(instance) {
        }

    private:
        GLFWwindow* m_Window = nullptr;
        std::vector<const char*> m_ShadersNames;
        std::vector<const char*> m_ResolutionsNames;
        std::vector<const char*> m_EncodersNames;
        std::vector<const char*> m_OutputFormatsNames;

        static std::vector<std::string> m_DroppedFiles;

        void RenderUI();
        void RenderVideoTable();
        void RenderSettings();
        void RenderLogs();
        void RenderProgress();

        void InitializeWindow();
        void ApplyStyle();
        static void DropCallback(GLFWwindow*, int, const char**);
    };
} // Upscaler

#endif //RENDERER_H
