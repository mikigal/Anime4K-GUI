#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <pch.h>

namespace Upscaler {
    class App;

    class Configuration {
    public:
        App* Instance;

        int Resolution = 0;
        int Shader = 0;
        int Encoder = 0;
        int Crf = 18;
        int Cq = 18;
        int CpuThreads = std::thread::hardware_concurrency();
        int OutputFormat = 0;
        int ConcurrentJobs = 1;
        bool DebugMode = false;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Configuration, Resolution, Shader, Encoder, Crf, Cq, CpuThreads, OutputFormat,
            ConcurrentJobs, DebugMode)

        void Load();
        void Save();

        Configuration(App* instance)
            : Instance(instance) {
        }
    };
}

#endif // CONFIGURATION_H