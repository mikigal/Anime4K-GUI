#include "App.h"

#include "Hardware/GpuDetector.h"
#include "Utilities/Utilities.h"

namespace Upscaler {
    void App::Init() {
        m_Logger.Init(false);
        GetLogger().Debug("Initialized logger");

        m_AssetLoader.Load("assets", "pak");
        m_Configuration.Load();

        m_GpuDetector.AnalyzeAvailableEncoders();

        Utilities::PreventSleep(m_Logger);

        bool result = m_Renderer.Init();
        if (!result) {
            exit(1);
            return;
        }

        m_Renderer.Terminate();
    }
}
