#ifndef ENCODER_H
#define ENCODER_H
#include <pch.h>

namespace Upscaler {
    class Encoder {
    public:
        std::string m_Name;
        std::string m_Value;
        std::string m_Vendor;
        bool m_CrfSupport = false;
        bool m_CqSupport = false;
        std::vector<std::string> m_HwaccelParams;
        std::vector<std::string> m_Params;

        Encoder(const std::string &m_name, const std::string &m_value, const std::string &m_vendor, bool m_crf_support,
                bool m_cq_support, const std::vector<std::string> &m_hwaccel_params,
                const std::vector<std::string> &m_params)
            : m_Name(m_name),
              m_Value(m_value),
              m_Vendor(m_vendor),
              m_CrfSupport(m_crf_support),
              m_CqSupport(m_cq_support),
              m_HwaccelParams(m_hwaccel_params),
              m_Params(m_params) {
        }
    };
}

#endif //ENCODER_H
