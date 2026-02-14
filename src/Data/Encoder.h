#ifndef ENCODER_H
#define ENCODER_H
#include <pch.h>

namespace Upscaler {
    struct Encoder {
        const std::string Name;
        const std::string Value;
        const std::string Vendor;
        const std::string Type;
        const bool CrfSupport;
        const bool CqSupport;
        const bool VideoToolboxCqSupport;
        const bool ThreadsLimitSupported;
        const std::vector<std::string> Params;
        const std::vector<std::string> ThreadsLimitParams;
        bool Available = false;
    };
}

#endif //ENCODER_H
