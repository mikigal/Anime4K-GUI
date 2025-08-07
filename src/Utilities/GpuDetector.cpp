#include "GpuDetector.h"
#include "pch.h"

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__linux__)
    #include <cstdio>
    #include <memory>
#endif

namespace Upscaler {
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