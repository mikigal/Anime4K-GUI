#ifndef UPSCALER_H
#define UPSCALER_H
#include "Video/VideoLoader.h"
#include "Data/Configuration.h"
#include "UI/Renderer.h"
#include "Utilities/AssetLoader.h"
#include "Utilities/Logger.h"

namespace Upscaler {
    class App {
    private:
        AssetLoader m_AssetLoader{this};
        VideoLoader m_VideoLoader{this};
        Configuration m_Configuration{this};
        Renderer m_Renderer{this};
        Logger m_Logger;

    public:
        void Init();

        AssetLoader& GetAssetLoader() { return m_AssetLoader; }
        VideoLoader& GetVideoLoader() { return m_VideoLoader; }
        Configuration& GetConfiguration() { return m_Configuration; }
        Renderer& GetRenderer() { return m_Renderer; }
        Logger& GetLogger() { return m_Logger; }
    };
}


#endif //UPSCALER_H
