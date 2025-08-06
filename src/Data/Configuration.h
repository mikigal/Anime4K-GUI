#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <pch.h>

#include "Encoder.h"
#include "Resolution.h"
#include "Shader.h"

namespace Upscaler {
    class App;

    class Configuration {
    public:
        App* Instance;
        std::vector<Encoder> Encoders;
        std::vector<Resolution> Resolutions;
        std::vector<Shader> Shaders;
        std::vector<std::string> OutputFormats;
        std::vector<std::string> CodecBlacklist;

        void Load();

        Configuration(App* instance)
            : Instance(instance) {
        }
    };
}

#endif //CONFIGURATION_H
