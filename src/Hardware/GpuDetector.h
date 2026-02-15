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

#ifdef _WIN32
        std::wstring GetDevicePropertyString(HDEVINFO hDevInfo, SP_DEVINFO_DATA& devInfoData, DWORD property);
#endif

        GpuDetector(App* instance)
            : Instance(instance) {
        }
    };
}


#endif // GPUDETECTOR_H