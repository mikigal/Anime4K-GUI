#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <pch.h>

#include "Encoder.h"
#include "Resolution.h"
#include "Shader.h"
#include "App.h"

namespace Upscaler {
    class Configuration {

    public:
        App& m_App;
        std::vector<Encoder> Encoders;
        std::vector<Resolution> Resolutions;
        std::vector<Shader> Shaders;
        std::vector<std::string> CodecBlacklist;
        void Load();

        Configuration(App &m_app)
            : m_App(m_app) {
        }
    };
}

#endif //CONFIGURATION_H
