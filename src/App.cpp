#include "App.h"

#include "Hardware/GpuDetector.h"

namespace Upscaler {
    void App::Init() {
        m_Logger.Init();
        GetLogger().Debug("Initialized logger");

        m_AssetLoader.Load("assets.pak");
        m_Configuration.Load();

        std::string a = "/home/mikigal/Documents/Another01.mp4";
        m_VideoLoader.loadVideo(a);

        std::vector<std::string> gpus = GpuDetector::FindGPUs();
        m_Logger.Info("Available GPUs:");
        for (const std::string& gpu : gpus) {
            m_Logger.Info("  {}", gpu);
        }

        PreventSleep();

        bool result = m_Renderer.Init();
        if (!result) {
            exit(1);
            return;
        }

        m_Renderer.Terminate();
    }

    void App::PreventSleep() {
#ifdef _WIN32
        if (!SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED)) {
            m_Logger.Error("Failed to block sleep/hibernation. Error code: {}", GetLastError());
        }
#else
        m_Logger.Warn("Sleep prevention is not supported on this operating system. Please ensure your computer does not go to sleep while processing videos.");
#endif

    }
}
