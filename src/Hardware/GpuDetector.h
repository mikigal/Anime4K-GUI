#ifndef GPUDETECTOR_H
#define GPUDETECTOR_H

namespace Upscaler {
    class App;

    class GpuDetector {
    public:
        App* Instance;
        std::vector<std::string> FindGPUs();
        std::string GetVendorFromDeviceID(const std::string&);
        void AnalyzeAvailableEncoders();

        GpuDetector(App* instance)
            : Instance(instance) {
        }
    };
}


#endif // GPUDETECTOR_H