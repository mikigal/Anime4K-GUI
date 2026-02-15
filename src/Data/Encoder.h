#ifndef ENCODER_H
#define ENCODER_H
#include <pch.h>
#include <utility>

namespace Upscaler {
    class Encoder {
    public:
        std::string Name;
        std::string Value;
        std::string Vendor;
        std::string Type;
        bool CrfSupport;
        bool CqSupport;
        bool VideoToolboxCqSupport;
        bool ThreadsLimitSupported;
        std::vector<std::string> Params;
        std::vector<std::string> ThreadsLimitParams;
        bool Available;

        Encoder(std::string name, std::string value, std::string vendor, std::string type,
                bool crfSupport, bool cqSupport, bool videoToolboxCqSupport, bool threadsLimitSupported,
                std::vector<std::string> params, std::vector<std::string> threadsLimitParams)
            : Name(std::move(name)), Value(std::move(value)), Vendor(std::move(vendor)),
              Type(std::move(type)), CrfSupport(crfSupport), CqSupport(cqSupport),
              VideoToolboxCqSupport(videoToolboxCqSupport), ThreadsLimitSupported(threadsLimitSupported),
              Params(std::move(params)), ThreadsLimitParams(std::move(threadsLimitParams)),
              Available(false) {}
    };
}

#endif //ENCODER_H
