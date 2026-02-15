#include "Configuration.h"
#include <pch.h>

#include "App.h"

#define CONFIG_NAME "config.json"

namespace Upscaler {

    void Configuration::Load() {
        if (!std::filesystem::exists(CONFIG_NAME)) {
            this->Instance->GetLogger().Debug("No configuration file found, saving default file");
            Save();
            return;
        }

        std::ifstream file(CONFIG_NAME);
        nlohmann::json json = nlohmann::json::parse(file);
        json.get_to(*this);
        file.close();
    }

    void Configuration::Save() {
        nlohmann::json json = *this;

        std::ofstream file(CONFIG_NAME);
        if (!file.is_open()) {
            this->Instance->GetLogger().Error("Could not open {} to save configuration", CONFIG_NAME);
            return;
        }

        file << json.dump(4);
        file.close();
    }

    // Look by names as SelectedEncoder is index of currently available encoders
    Encoder& Configuration::GetSelectedEncoder() const {
        for (Encoder& encoder : Instance->GetData().GetEncoders()) {
            if (encoder.GetName() == Instance->GetData().GetEncodersNames()[m_Encoder]) {
                return encoder;
            }
        }

        throw std::runtime_error("Encoder " + std::string(Instance->GetData().GetEncodersNames()[Instance->GetConfiguration().m_Encoder]) + " does not exist");
    }

    Resolution& Configuration::GetSelectedResolution() const {
        return Instance->GetData().GetResolutions()[m_Resolution];
    }

    Shader& Configuration::GetSelectedShader() const {
        return Instance->GetData().GetShaders()[m_Shader];
    }

    std::string& Configuration::GetSelectedOutputFormat() const {
        return Instance->GetData().GetOutputFormats()[m_OutputFormat];
    }
}
