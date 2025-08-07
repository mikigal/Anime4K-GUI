#ifndef GPUDETECTOR_H
#define GPUDETECTOR_H

namespace Upscaler {
    class GpuDetector {
    public:
        static std::vector<std::string> FindGPUs();
    };
}


#endif // GPUDETECTOR_H