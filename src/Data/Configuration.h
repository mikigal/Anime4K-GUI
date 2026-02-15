#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <pch.h>

#include "Encoder.h"
#include "Resolution.h"
#include "Shader.h"

namespace Upscaler {
    class App;

    class Configuration {
    private:
        App* Instance;

    public:
        // Renderer needs pointer to these fields
        int m_Resolution = 0;
        int m_Shader = 0;
        int m_Encoder = 0;
        int m_Crf = 18;
        int m_Cq = 18;
        int m_CpuThreads = std::thread::hardware_concurrency();
        int m_OutputFormat = 0;
        int m_ConcurrentJobs = 1;
        bool m_DebugMode = false;

        void Load();
        void Save();

        Configuration(App* instance)
            : Instance(instance) {
        }

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Configuration, m_Resolution, m_Shader, m_Encoder, m_Crf, m_Cq, m_CpuThreads, m_OutputFormat,
            m_ConcurrentJobs, m_DebugMode)


        Resolution& GetSelectedResolution() const;
        Encoder& GetSelectedEncoder() const;
        Shader& GetSelectedShader() const;
        std::string& GetSelectedOutputFormat() const;
    };
}

#endif // CONFIGURATION_H