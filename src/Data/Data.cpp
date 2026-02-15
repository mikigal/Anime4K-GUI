#include "Data.h"
#include <pch.h>
#include "App.h"
#include "Utilities/AssetLoader.h"

namespace Upscaler {
    void Data::Load() {
        AssetLoader::AssetData data = Instance->GetAssetLoader().GetFileData("data.json");
        nlohmann::json json = nlohmann::json::parse(data.begin(), data.end());

        for (nlohmann::basic_json<>& item: json["resolutions"]) {
            Resolution resolution(
                item.at("width"),
                item.at("height"),
                item.at("aspect_ratio")
            );
            m_Resolutions.push_back(resolution);
        }
        Instance->GetLogger().Debug("Loaded {} resolutions", m_Resolutions.size());

        for (nlohmann::basic_json<>& item: json["shaders"]) {
            Shader shader(
                item.at("name"),
                item.at("path")
            );
            m_Shaders.push_back(shader);
        }
        Instance->GetLogger().Debug("Loaded {} shaders", m_Shaders.size());

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
            m_Encoders.push_back(encoder);
        }
        Instance->GetLogger().Debug("Loaded {} encoders", m_Encoders.size());

        m_OutputFormats = json["output_formats"].get<std::vector<std::string> >();
        Instance->GetLogger().Debug("Loaded {} output formats", m_OutputFormats.size());

        Instance->GetLogger().Debug("Loaded application data");
    }

    void Data::LoadNames() {
        for (Shader& shader : Instance->GetData().GetShaders()) {
            m_ShadersNames.push_back(shader.GetName().c_str());
        }

        for (Resolution& resolution : Instance->GetData().GetResolutions()) {
            m_ResolutionsNames.push_back(resolution.GetVisibleName().c_str());
        }

        for (std::string& outputFormat : Instance->GetData().GetOutputFormats()) {
            m_OutputFormatsNames.push_back(outputFormat.c_str());
        }

        for (Encoder& encoder : Instance->GetData().GetEncoders()) {
            if (!encoder.IsAvailable()) {
                continue;
            }

            m_EncodersNames.push_back(encoder.GetName().c_str());
        }
    }
}
