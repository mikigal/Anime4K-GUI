#include "Configuration.h"
#include <pch.h>

#include "Utilities/AssetLoader.h"

namespace Upscaler {

    void Configuration::Load() {
        AssetLoader::AssetData data = m_App.GetAssetLoader().GetFileData("data.json");
        nlohmann::json json = nlohmann::json::parse(data.begin(), data.end());

        for (nlohmann::basic_json<>& item : json["resolutions"]) {
            Resolution resolution(
                item.at("width"),
                item.at("height"),
                item.at("aspect_ratio")
            );
            Resolutions.push_back(resolution);
        }
        LOG_DEBUG("Loaded {} resolutions", Resolutions.size());

        for (nlohmann::basic_json<>& item : json["shaders"]) {
            Shader shader(
                item.at("name"),
                item.at("path")
            );
            Shaders.push_back(shader);
        }
        LOG_DEBUG("Loaded {} shaders", Shaders.size());

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
            Encoders.push_back(encoder);
        }
        LOG_DEBUG("Loaded {} encoders", Encoders.size());

        CodecBlacklist = json["codec_blacklist"].get<std::vector<std::string>>();
        LOG_DEBUG("Loaded {} blacklisted codecs", CodecBlacklist.size());

        LOG_INFO("Loaded application config");
    }
}
