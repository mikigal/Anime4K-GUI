#ifndef ENCODER_H
#define ENCODER_H
#include <pch.h>

namespace Upscaler {
    struct Encoder {
        const std::string Name;
        const std::string Value;
        const std::string Vendor;
        const bool CrfSupport;
        const bool CqSupport;
        const std::vector<std::string> HwaccelParams;
        const std::vector<std::string> Params;
    };
}

#endif //ENCODER_H
