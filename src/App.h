#ifndef UPSCALER_H
#define UPSCALER_H
#include "Data/Configuration.h"
#include "Data/Data.h"
#include "Video/VideoLoader.h"
#include "Hardware/GpuDetector.h"
#include "UI/Renderer.h"
#include "Utilities/AssetLoader.h"
#include "Utilities/Logger.h"
#include "Video/VideoProcessor.h"

namespace Upscaler {
    class App {
    private:
        Configuration m_Configuration{this};
        AssetLoader m_AssetLoader{this};
        VideoLoader m_VideoLoader{this};
        VideoProcessor m_VideoProcessor{this};
        Data m_Data{this};
        Renderer m_Renderer{this};
        GpuDetector m_GpuDetector{this};
        Logger m_Logger;

    public:
        void Init();

        Configuration& GetConfiguration() { return m_Configuration; }
        AssetLoader& GetAssetLoader() { return m_AssetLoader; }
        VideoLoader& GetVideoLoader() { return m_VideoLoader; }
        VideoProcessor& GetVideoProcessor() { return m_VideoProcessor; }
        Data& GetData() { return m_Data; }
        Renderer& GetRenderer() { return m_Renderer; }
        GpuDetector& GetGpuDetector() { return m_GpuDetector; }
        Logger& GetLogger() { return m_Logger; }
    };
}


#endif //UPSCALER_H
