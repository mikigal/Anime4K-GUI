#include "GpuDetector.h"

#include "App.h"
#include "pch.h"
#include "Data/Encoder.h"
#include "Utilities/Logger.h"

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__linux__)
    #include <cstdio>
    #include <memory>
#endif

namespace Upscaler {

    void GpuDetector::AnalyzeAvailableEncoders() {
        std::vector<std::string> gpus = FindGPUs();
        Instance->GetLogger().Info("Available GPUs:");

        bool nvidia = false;
        bool amd = false;
        bool av1Supported = false;
        for (std::string gpu : gpus) {
            Instance->GetLogger().Info("  - {}", gpu);
            std::ranges::transform(gpu, gpu.begin(),
                                   [](const unsigned char c) { return std::tolower(c); });

            if (gpu.find("advanced micro devices") != std::string::npos) {
                amd = true;
                // Only RX 7000 and RX 9000 support AV1 encoding
                av1Supported = gpu.find("rx 7") != std::string::npos || gpu.find("rx 9") != std::string::npos;
            }

            if (gpu.find("nvidia") != std::string::npos) {
                nvidia = true;
                // Only RTX 4000 and RTX 5000 support AV1 encoding
                av1Supported = gpu.find("rtx 40") != std::string::npos || gpu.find("rtx 50") != std::string::npos;
            }

            // Check for AMD iGPU
            if (nvidia && amd) {
                Instance->GetLogger().Info("Found AMD iGPU, ignoring it");
                amd = false;
            }
        }

        Instance->GetLogger().Debug("Available encoders:");
        for (Encoder& encoder : Instance->GetConfiguration().Encoders) {
            if (encoder.Vendor == "cpu") {
                encoder.Available = true;
            }

            if (encoder.Vendor == "nvidia" && nvidia && (encoder.Type != "av1" || (encoder.Type == "av1" && av1Supported))) {
                encoder.Available = true;
            }

            if (encoder.Vendor == "advanced micro devices" && amd && (encoder.Type != "av1" || (encoder.Type == "av1" && av1Supported))) {
                encoder.Available = true;
            }

            if (encoder.Available) {
                Instance->GetLogger().Debug("  - {}", encoder.Name);
            }
        }
    }


    std::vector<std::string> GpuDetector::FindGPUs() {
        std::vector<std::string> gpus;

#if defined(_WIN32)
        DISPLAY_DEVICE dd;
        dd.cb = sizeof(dd);
        int deviceIndex = 0;

        while (EnumDisplayDevices(nullptr, deviceIndex, &dd, 0)) {
            if (dd.StateFlags & DISPLAY_DEVICE_ACTIVE || dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) {
                std::string model = dd.DeviceString;
                gpus.push_back(model);
            }
            deviceIndex++;
        }

#elif defined(__linux__)
        FILE* pipe = popen("lspci", "r");
        if (!pipe) {
            std::cout << "can't execute lspci" << std::endl;
            return gpus;
        }

        char buffer[512];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            std::string line(buffer);
            if (line.find("VGA") == std::string::npos &&
            line.find("3D") == std::string::npos &&
            line.find("Display") == std::string::npos) {
                continue;
            }

            int secondColon = line.find(':', line.find(':') + 1);
            if (secondColon == std::string::npos) {
                std::cout << "weird lspci entry " << line << std::endl;
            }

            std::string description = line.substr(secondColon + 1);
            description.erase(0, description.find_first_not_of(" \t"));
            description.erase(description.find_last_not_of(" \n\r\t") + 1);
            gpus.push_back(description);
        }

        pclose(pipe);
#elif defined(__APPLE__)
        gpus.push_back("Apple Silicon GPU");
#endif

        return gpus;
    }
}