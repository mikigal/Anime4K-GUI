#include "App.h"

namespace Upscaler {
    void App::Init() {
        Logger::Init();
        LOG_DEBUG("Initialized logger");

        m_AssetLoader.Load("assets.pak");
        m_Configuration.Load();

        std::string a = "/home/mikigal/Documents/Another01.mp4";
        m_VideoLoader.loadVideo(a);

        bool result = m_Renderer.Init();
        if (!result) {
            exit(1);
            return;
        }

        m_Renderer.Terminate();
    }
}
