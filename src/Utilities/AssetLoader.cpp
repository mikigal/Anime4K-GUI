#include "AssetLoader.h"
#include "App.h"

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace Upscaler {

#ifdef __APPLE__
    std::string GetResoucePathCF(const char* name, const char* ext) {
        CFBundleRef bundle = CFBundleGetMainBundle();
        if (!bundle) return {};
        CFStringRef cfName = CFStringCreateWithCString(nullptr, name, kCFStringEncodingUTF8);
        CFStringRef cfExt  = CFStringCreateWithCString(nullptr, ext,  kCFStringEncodingUTF8);
        CFURLRef url = CFBundleCopyResourceURL(bundle, cfName, cfExt, nullptr);
        CFRelease(cfName);
        CFRelease(cfExt);
        if (!url) return {};
        char path[PATH_MAX];
        if (!CFURLGetFileSystemRepresentation(url, true, (UInt8*)path, sizeof(path))) {
            CFRelease(url);
            return {};
        }
        CFRelease(url);
        return path;
    }
#endif

    void AssetLoader::Load(const std::string& filename, const std::string& ext) {
        Instance->GetLogger().Debug("Loading pak file: {}", filename);


        std::string path = filename + "." + ext;

#ifdef __APPLE__
        path = GetResoucePathCF("assets", "pak");
#endif

        std::ifstream file(path, std::ios::binary);
        if (!file)
            throw std::runtime_error("Failed to open pak file: " + filename);

        Instance->GetLogger().Assert(file.is_open(), "Pak file is not open: " + filename);

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

        for (const auto& [filename, data]: m_Files) {
            Instance->GetLogger().Debug("File: {}, Size: {} bytes", filename, data.size());
        }

        Instance->GetLogger().Info("Loaded {}", filename);
    }

    bool AssetLoader::HasFile(const std::string& filename) const {
        return m_Files.contains(filename);
    }

    const AssetLoader::AssetData& AssetLoader::GetFileData(const std::string& filename) const {
        const auto it = m_Files.find(filename);
        Instance->GetLogger().Assert(it != m_Files.end(), "File not found in pak: " + filename);
        return it->second;
    }
}