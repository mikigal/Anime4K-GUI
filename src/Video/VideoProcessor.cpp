#include "VideoProcessor.h"

#include "App.h"
#include "pch.h"
#include "Utilities/Utilities.h"

namespace Upscaler {

    void VideoProcessor::ValidateFFmpeg() {
        std::string output;

        TinyProcessLib::Process process(
            "ffmpeg -version", "",
            [&output](const char* bytes, size_t n) {
                output.append(bytes, n);
            },
            [this](const char* bytes, size_t n) {
                std::string errorMessage = std::string(bytes, n);
                while (!errorMessage.empty() && (errorMessage.back() == '\n' || errorMessage.back() == '\r')) {
                    errorMessage.pop_back();
                }

                Instance->GetLogger().Critical("An error occurred while executing: ffmpeg -version, stderr: {}", errorMessage);
                Instance->GetLogger().Critical("There's problem with your FFMPEG installation, probably it wasn't found");
#ifdef _WIN32
                this->Instance->GetLogger().Critical("Make sure bundled FFMPEG is in the same directory as Anime4K-GUI.exe\n");
#elif __linux__
                Instance->GetLogger().Critical("Please install FFMPEG using your package manager and make sure it's available in $PATH\n");
#elif __APPLE__
                Instance->GetLogger().Critical("Please install FFMPEG using brew package manager and make sure it's available in $PATH\n");
#endif
                Instance->GetRenderer().CriticalError = true;
            });

        int exitCode = process.get_exit_status();
        Instance->GetLogger().Debug("ffmpeg -version exited with code {}", exitCode);
        if (exitCode != 0) {
            return;
        }

        std::string version = Utilities::Split(output, "Copyright")[0];
        Instance->GetLogger().Info("{}\n", version);
        if (output.find("enable-libplacebo") == std::string::npos) {
            Instance->GetLogger().Critical("Your FFMPEG build does not include libplacebo, application will NOT work");
            Instance->GetLogger().Critical("Details about your FFMPEG: \n{}", output);
            Instance->GetRenderer().CriticalError = true;
        }
    }

}
