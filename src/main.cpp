#include "pch.h"
#include "UI/Renderer.h"
#include "Utilities/AssetLoader.h"
#include "Utilities/Logger.h"

void RenderUI();

int main() {
    Upscaler::AssetLoader::Get().Load("assets.pak");
    Upscaler::Logger::Info("Loaded assets");

    Upscaler::Renderer renderer;
    bool result = renderer.Initialize();
    if (!result) {
        return 1;
    }

    renderer.Terminate();
    return 0;
}
