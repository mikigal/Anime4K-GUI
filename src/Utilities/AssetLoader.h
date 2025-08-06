#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include "pch.h"

namespace Upscaler {
    class App;
    class AssetLoader {
    public:
        App* Instance;
        typedef std::vector<char> AssetData;

        void Load(const std::string& filename);
        bool HasFile(const std::string& filename) const;
        const AssetData& GetFileData(const std::string& filename) const;

        AssetLoader(App* instance)
            : Instance(instance) {
        }

    private:
        std::unordered_map<std::string, AssetData> m_Files;
    };
} // namespace Upscaler

#endif // ASSETLOADER_H
