#include "App.h"

#include "Hardware/GpuDetector.h"
#include "Utilities/Utilities.h"

#define VERSION "2.0.0"

namespace Upscaler {
    void App::Init() {
        m_Configuration.Load();
        m_Logger.Init(m_Configuration.m_DebugMode);
        GetLogger().Info("Anime4K-GUI v{}", VERSION);
        GetLogger().Info("Anime upscaler GUI application based on Anime4K shaders");
        GetLogger().Info("Website: https://github.com/mikigal/Anime4K-GUI");
        GetLogger().Info("Drag and drop your files to start upscaling\n");

        GetLogger().Debug("Initialized logger");

        m_AssetLoader.Load("assets", "pak");
        m_Data.Load();

        m_GpuDetector.AnalyzeAvailableEncoders();
        m_VideoProcessor.ValidateFFmpeg();
        m_Data.LoadNames();

        Utilities::PreventSleep(m_Logger);

        bool result = m_Renderer.Init();
        if (!result) {
            exit(1);
            return;
        }

        m_VideoProcessor.CancelProcessing();
        m_Configuration.Save();
        m_Renderer.Terminate();
        m_Logger.Shutdown();
    }
}
