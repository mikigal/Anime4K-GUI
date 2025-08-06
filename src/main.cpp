#include <sys/syslog.h>

#include "VideoLoader.h"
#include "Data/Configuration.h"
#include "UI/Renderer.h"
#include "Utilities/AssetLoader.h"
#include "Utilities/Logger.h"

int main() {
    Upscaler::Logger::Init();
    LOG_DEBUG("Initialized logger");

    Upscaler::AssetLoader::Get().Load("assets.pak");
    Upscaler::Logger::Info("Loaded assets");

    Upscaler::Configuration config;
    config.Load();

    std::string a = "/home/mikigal/Documents/Another01.mp4";
    Upscaler::VideoLoader videoLoader;
    videoLoader.loadVideo(a);

    Upscaler::Renderer renderer;
    bool result = renderer.Initialize();
    if (!result) {
        return 1;
    }

    renderer.Terminate();
    return 0;
}
