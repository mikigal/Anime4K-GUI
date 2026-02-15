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
}
