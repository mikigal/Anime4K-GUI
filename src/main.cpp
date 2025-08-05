#include "pch.h"
#include "UI/Renderer.h"
#include "Utilities/AssetLoader.h"
#include "Utilities/Logger.h"

void RenderUI();

int main() {
    Upscaler::AssetLoader::Get().Load("assets.pak");
    Upscaler::Logger::Info("Loaded assets");

    TinyProcessLib::Process process("ffprobe -v quiet -print_format json -show_format -show_streams /Users/mikigal/Desktop/Another01.mp4", "",
    [](const char *bytes, size_t n) {
        std::cout << "Output from stdout: " << std::string(bytes, n);
    },
    [](const char *bytes, size_t n) {
        std::cout << "Output from stderr: " << std::string(bytes, n);
    });

    int exitCode = process.get_exit_status();
    printf("Exit code: %d", exitCode);

    //Upscaler::Renderer renderer;
    //bool result = renderer.Initialize();
    //if (!result) {
    //    return 1;
    //}

    //renderer.Terminate();
    return 0;
}
