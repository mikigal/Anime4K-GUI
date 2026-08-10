#ifndef GPUDETECTOR_H
#define GPUDETECTOR_H

#ifdef _WIN32
    #include <setupapi.h>
#endif

namespace Upscaler {
    class App;

    class GpuDetector {
    public:
        App* Instance;
        std::vector<std::string> FindGPUs();
        std::string GetVendorFromDeviceID(const std::string&);
        void AnalyzeAvailableEncoders();

        // Name of the discrete GPU (Nvidia/AMD) that should be used for Vulkan-based
        // shader upscaling, so it isn't left to the Vulkan loader's default enumeration
        // order (which may pick an integrated GPU on hybrid-GPU/Optimus laptops).
        // Empty if no discrete GPU was found, in which case Vulkan device selection
        // falls back to the default behavior.
        const std::string& GetPreferredVulkanDeviceName() const { return m_PreferredVulkanDeviceName; }

#ifdef _WIN32
        std::wstring GetDevicePropertyString(HDEVINFO hDevInfo, SP_DEVINFO_DATA& devInfoData, DWORD property);
#endif

        GpuDetector(App* instance)
            : Instance(instance) {
        }

    private:
        std::string m_PreferredVulkanDeviceName;
    };
}


#endif // GPUDETECTOR_H