#include "App.h"

#include "Hardware/GpuDetector.h"
#include "Utilities/Utilities.h"

namespace Upscaler {
    void App::Init() {
        m_Configuration.Load();
        m_Logger.Init(m_Configuration.DebugMode);

        GetLogger().Debug("Initialized logger");

        m_AssetLoader.Load("assets", "pak");
        m_Data.Load();

        m_VideoProcessor.ValidateFFmpeg();
        m_GpuDetector.AnalyzeAvailableEncoders();

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
