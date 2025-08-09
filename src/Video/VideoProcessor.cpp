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
            [](const char* bytes, size_t n) {
                std::cout << "An error occurred while executing ffmpeg -version, stderr: " << std::string(bytes, n) << std::endl;
            });

        int exitCode = process.get_exit_status();
        Instance->GetLogger().Debug("ffmpeg -version exited with code {}", exitCode);
        if (exitCode != 0) {
            Instance->GetLogger().Error("ffmpeg not found");
            return;
        }

        std::string version = Utilities::Split(output, "Copyright")[0];
        Instance->GetLogger().Info("{}", version);
        if (output.find("enable-libplacebo") == std::string::npos) {
            Instance->GetLogger().Critical("Your FFMPEG build does not include libplacebo, application will NOT work");
            Instance->GetLogger().Critical("Details about your FFMPEG: \n{}", output);
        }
    }

}
