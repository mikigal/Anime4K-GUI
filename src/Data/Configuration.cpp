#include "Configuration.h"
#include <pch.h>
#include "App.h"
#include "Utilities/AssetLoader.h"

namespace Upscaler {
    void Configuration::Load() {
        AssetLoader::AssetData data = Instance->GetAssetLoader().GetFileData("data.json");
        nlohmann::json json = nlohmann::json::parse(data.begin(), data.end());

        for (nlohmann::basic_json<>& item: json["resolutions"]) {
            Resolution resolution(
                item.at("width"),
                item.at("height"),
                item.at("aspect_ratio")
            );
            Resolutions.push_back(resolution);
        }
        Instance->GetLogger().Debug("Loaded {} resolutions", Resolutions.size());

        for (nlohmann::basic_json<>& item: json["shaders"]) {
            Shader shader(
                item.at("name"),
                item.at("path")
            );
            Shaders.push_back(shader);
        }
        Instance->GetLogger().Debug("Loaded {} shaders", Shaders.size());

        for (nlohmann::basic_json<>& item: json["encoders"]) {
            Encoder encoder(
                item.at("name"),
                item.at("value"),
                item.at("vendor"),
                item.at("type"),
                item.at("crf_supported"),
                item.at("cq_supported"),
                item.at("videotoolbox_cq_supported"),
                item.at("threads_limit_supported"),
                item.at("params").get<std::vector<std::string> >(),
                item.at("threads_limit_params").get<std::vector<std::string> >()
            );
            Encoders.push_back(encoder);
        }
        Instance->GetLogger().Debug("Loaded {} encoders", Encoders.size());

        OutputFormats = json["output_formats"].get<std::vector<std::string> >();
        Instance->GetLogger().Debug("Loaded {} output formats", OutputFormats.size());

        Instance->GetLogger().Debug("Loaded application config");
    }
}
