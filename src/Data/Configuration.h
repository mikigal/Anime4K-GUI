#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <pch.h>

#include "Encoder.h"
#include "Resolution.h"
#include "Shader.h"

namespace Upscaler {
    class Configuration {
    public:
        std::vector<Encoder> m_Encoders;
        std::vector<Resolution> m_Resolutions;
        std::vector<Shader> m_Shaders;
        std::vector<std::string> m_CodecBlacklist;
        void Load();
    };
}

#endif //CONFIGURATION_H
