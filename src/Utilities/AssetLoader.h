#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include "pch.h"
#include "Logger.h"

#define GET_ASSET(filename) Upscaler::AssetLoader::Get().GetFileData(filename)

namespace Upscaler {
class AssetLoader {
public:
    typedef std::vector<char> AssetData;

    void Load(const std::string& filename) {
        LOG_DEBUG("Loading pak file: {}", filename);
        std::ifstream file(filename, std::ios::binary);
        if (!file)
            throw std::runtime_error("Failed to open pak file: " + filename);

        ASSERT(file.is_open(), "Pak file is not open: " + filename);

        uint32_t fileCount = 0;
        file.read(reinterpret_cast<char*>(&fileCount), sizeof(fileCount));
        for (uint32_t i = 0; i < fileCount; ++i) {
            uint16_t pathLen = 0;
            file.read(reinterpret_cast<char*>(&pathLen), sizeof(pathLen));

            std::string path(pathLen, '\0');
            file.read(path.data(), pathLen);

            uint32_t dataLen = 0;
            file.read(reinterpret_cast<char*>(&dataLen), sizeof(dataLen));

            AssetData data(dataLen);
            file.read(data.data(), dataLen);

            m_Files[path] = std::move(data);
        }

        for (const auto& [filename, data] : m_Files) {
            LOG_DEBUG("File: {}, Size: {} bytes", filename, data.size());
        }

        LOG_INFO("Loaded {}", filename);
    }

    bool HasFile(const std::string& filename) const {
        return m_Files.contains(filename);
    }

    const AssetData& GetFileData(const std::string& filename) const {
        const auto it = m_Files.find(filename);
        ASSERT(it != m_Files.end(), "File not found in pak: " + filename);
        return it->second;
    }

private:
    std::unordered_map<std::string, AssetData> m_Files;
};
} // namespace Upscaler

#endif // ASSETLOADER_H