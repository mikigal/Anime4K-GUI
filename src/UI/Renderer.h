#ifndef RENDERER_H
#define RENDERER_H
#include "Data/Encoder.h"
#include "Data/Resolution.h"
#include "Data/Shader.h"

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
        int SelectedCrf = 18;
        int SelectedCq = 18;
        int SelectedCpuThreads = std::thread::hardware_concurrency();
        int SelectedOutputFormat = 0;
        bool SelectedDebugMode = false;

        static std::string Logs;

        bool CriticalError = false;

        bool Init();
        void Terminate();
        Resolution& GetSelectedResolution();
        Encoder& GetSelectedEncoder();
        Shader& GetSelectedShader();
        std::string& GetSelectedOutputFormat();

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
        void RenderProgress(float);

        void InitializeWindow();
        void ApplyStyle();
        static void DropCallback(GLFWwindow*, int, const char**);
    };
} // Upscaler

#endif //RENDERER_H
