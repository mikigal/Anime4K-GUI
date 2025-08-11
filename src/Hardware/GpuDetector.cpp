#include "GpuDetector.h"

#include <regex>

#include "App.h"
#include "pch.h"
#include "Data/Encoder.h"
#include "Utilities/Logger.h"

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__linux__)
    #include <cstdio>
    #include <memory>
#elif defined(__APPLE__)
    #include <sys/sysctl.h>
#endif

namespace Upscaler {

#if defined(__APPLE__)

#endif

    void GpuDetector::AnalyzeAvailableEncoders() {
        std::vector<std::string> gpus = FindGPUs();
        Instance->GetLogger().Info("Available GPUs:");

        bool nvidia = false;
        bool amd = false;
        bool intel = false;
        bool apple = false;
        bool av1Supported = false;
        for (int i = 0; i < gpus.size(); i++) {
            std::string& gpu = gpus[i];
            Instance->GetLogger().Info("  {}. {} {}", i + 1, gpu, (i == gpus.size() - 1 ? "\n" : ""));
            std::ranges::transform(gpu, gpu.begin(),
                                   [](const unsigned char c) { return std::tolower(c); });

            // Fix potential vendor name mismatch
            std::string from = "advanced micro devices";
            int pos = gpu.find(from);
            if (pos != std::string::npos) {
                gpu.replace(pos, from.length(), "amd");
            }

            if (gpu.find("amd") != std::string::npos) {
                amd = true;
                // Only RX 7000 and RX 9000 support AV1 encoding
                av1Supported = gpu.find("rx 7") != std::string::npos || gpu.find("rx 9") != std::string::npos;
            }

            if (gpu.find("nvidia") != std::string::npos) {
                nvidia = true;
                // Only RTX 4000 and RTX 5000 support AV1 encoding
                av1Supported = gpu.find("rtx 40") != std::string::npos || gpu.find("rtx 50") != std::string::npos;
            }

            if (gpu.find("intel") != std::string::npos) {
                intel = true;
            }

            if (gpu.find("apple") != std::string::npos) {
                apple = true;
            }
        }

        // Check for iGPU
        if (nvidia && amd) {
            Instance->GetLogger().Info("Found AMD iGPU, ignoring it");
            amd = false;
        }

        if (nvidia && intel || amd && intel) {
            Instance->GetLogger().Info("Found Intel iGPU, ignoring it");
            intel = false;
        }

        if (intel) {
            Instance->GetLogger().Warn("Found Intel dGPU which is not supported. Only CPU based encoders will be available");
        }

#ifdef __linux__
        Instance->GetLogger().Warn("NVIDIA GPUs on Linux are supported only with nvidia-open drivers. If you encounter any issues make sure you use correct drivers");
#endif
        Instance->GetLogger().Debug("Available encoders:");
        for (Encoder& encoder : Instance->GetConfiguration().Encoders) {
            if (encoder.Vendor == "cpu") {
                encoder.Available = true;
            }

            if (encoder.Vendor == "nvidia" && nvidia && (encoder.Type != "av1" || (encoder.Type == "av1" && av1Supported))) {
                encoder.Available = true;
            }

            if (encoder.Vendor == "amd" && amd && (encoder.Type != "av1" || (encoder.Type == "av1" && av1Supported))) {
                encoder.Available = true;
            }

            if (encoder.Vendor == "apple" && apple) {
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
                std::string deviceID = dd.DeviceID;
                std::string vendor = GetVendorFromDeviceID(deviceID);
                gpus.push_back(vendor + " " + model);
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

        auto GetFromSysctl = [](const char* name) -> std::string {
            size_t size = 0;
            if (sysctlbyname(name, nullptr, &size, nullptr, 0) != 0 || size == 0) return {};
            std::string s(size, '\0');
            if (sysctlbyname(name, s.data(), &size, nullptr, 0) != 0) return {};
            if (!s.empty() && s.back() == '\0') s.pop_back();
            return s;
        };

        std::string arch = GetFromSysctl("hw.machine");

        if (arch == "arm64") {
            std::string cpu = GetFromSysctl("machdep.cpu.brand_string") == "" ? GetFromSysctl("machdep.cpu.brand_string") : GetFromSysctl("machdep.cpu.brand_string") ;
            gpus.push_back(cpu);
        }
        else {
            Instance->GetLogger().Warn("Hardware encoders on Intel based Macs are not supported");
        }

#endif

        return gpus;
    }

    std::string GpuDetector::GetVendorFromDeviceID(const std::string& deviceID) {
        std::regex venRegex("VEN_([0-9A-Fa-f]{4})");
        std::smatch match;

        if (std::regex_search(deviceID, match, venRegex)) {
            std::string venID = match[1];
            if (venID == "10DE") return "NVIDIA";
            if (venID == "1002") return "AMD";
            if (venID == "8086") return "Intel";
            return "UnknownVendor(" + venID + ")";
        }

        return "UnknownVendor";
    }
}