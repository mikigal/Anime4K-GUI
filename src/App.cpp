#include "App.h"

#include "Utilities/GpuDetector.h"

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

        bool result = m_Renderer.Init();
        if (!result) {
            exit(1);
            return;
        }

        m_Renderer.Terminate();
    }
}
