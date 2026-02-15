#ifndef DATA_H
#define DATA_H
#include <pch.h>

#include "Encoder.h"
#include "Resolution.h"
#include "Shader.h"

namespace Upscaler {
    class App;

    class Data {
    private:
        App* Instance;
        std::vector<Encoder> m_Encoders;
        std::vector<Resolution> m_Resolutions;
        std::vector<Shader> m_Shaders;
        std::vector<std::string> m_OutputFormats;

        std::vector<const char*> m_ShadersNames;
        std::vector<const char*> m_ResolutionsNames;
        std::vector<const char*> m_EncodersNames;
        std::vector<const char*> m_OutputFormatsNames;

    public:
        Data(App* instance)
            : Instance(instance) {
        }

        void Load();
        void LoadNames();

        [[nodiscard]] std::vector<Encoder>& GetEncoders() {
            return m_Encoders;
        }

        [[nodiscard]] std::vector<Resolution>& GetResolutions() {
            return m_Resolutions;
        }

        [[nodiscard]] std::vector<Shader>& GetShaders() {
            return m_Shaders;
        }

        [[nodiscard]] std::vector<std::string>& GetOutputFormats() {
            return m_OutputFormats;
        }

        [[nodiscard]] std::vector<const char*>& GetShadersNames() {
            return m_ShadersNames;
        }

        [[nodiscard]] std::vector<const char*>& GetResolutionsNames() {
            return m_ResolutionsNames;
        }

        [[nodiscard]] std::vector<const char*>& GetEncodersNames() {
            return m_EncodersNames;
        }

        [[nodiscard]] std::vector<const char*>& GetOutputFormatsNames() {
            return m_OutputFormatsNames;
        }
    };
}

#endif // DATA_H
