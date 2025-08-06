#include "Configuration.h"
#include <pch.h>

#include "Utilities/AssetLoader.h"

namespace Upscaler {
    static Configuration* config;

    void Configuration::Load() {
        config = this;

        AssetLoader::AssetData data = AssetLoader::Get().GetFileData("data.json");
        nlohmann::json json = nlohmann::json::parse(data.begin(), data.end());

        for (nlohmann::basic_json<>& item : json["resolutions"]) {
            Resolution resolution(
                item.at("width"),
                item.at("height"),
                item.at("aspect_ratio")
            );
            m_Resolutions.push_back(resolution);
        }
        LOG_DEBUG("Loaded {} resolutions", m_Resolutions.size());

        for (nlohmann::basic_json<>& item : json["shaders"]) {
            Shader shader(
                item.at("name"),
                item.at("path")
            );
            m_Shaders.push_back(shader);
        }
        LOG_DEBUG("Loaded {} shaders", m_Shaders.size());

        for (nlohmann::basic_json<>& item : json["encoders"]) {
            Encoder encoder(
                item.at("name"),
                item.at("value"),
                item.at("vendor"),
                item.at("crf_supported"),
                item.at("cq_supported"),
                item.at("hwaccel_params").get<std::vector<std::string>>(),
                item.at("params").get<std::vector<std::string>>()
            );
            m_Encoders.push_back(encoder);
        }
        LOG_DEBUG("Loaded {} encoders", m_Encoders.size());

        m_CodecBlacklist= json["codec_blacklist"].get<std::vector<std::string>>();
        LOG_DEBUG("Loaded {} blacklisted codecs", m_CodecBlacklist.size());

        LOG_INFO("Loaded application config");
    }
}
