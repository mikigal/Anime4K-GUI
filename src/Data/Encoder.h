#ifndef ENCODER_H
#define ENCODER_H
#include <pch.h>
#include <utility>

namespace Upscaler {
    class Encoder {
    private:
        std::string m_Name;
        std::string m_Value;
        std::string m_Vendor;
        std::string m_Type;
        bool m_CrfSupport;
        bool m_CqSupport;
        bool m_VideoToolboxCqSupport;
        bool m_ThreadsLimitSupported;
        std::vector<std::string> m_Params;
        std::vector<std::string> m_ThreadsLimitParams;
        bool m_Available;

    public:
        Encoder(std::string name, std::string value, std::string vendor, std::string type,
                bool crfSupport, bool cqSupport, bool videoToolboxCqSupport, bool threadsLimitSupported,
                std::vector<std::string> params, std::vector<std::string> threadsLimitParams)
            : m_Name(std::move(name)), m_Value(std::move(value)), m_Vendor(std::move(vendor)),
              m_Type(std::move(type)), m_CrfSupport(crfSupport), m_CqSupport(cqSupport),
              m_VideoToolboxCqSupport(videoToolboxCqSupport), m_ThreadsLimitSupported(threadsLimitSupported),
              m_Params(std::move(params)), m_ThreadsLimitParams(std::move(threadsLimitParams)),
              m_Available(false) {}

        [[nodiscard]] const std::string& GetName() const {
            return m_Name;
        }

        [[nodiscard]] std::string GetValue() const {
            return m_Value;
        }

        [[nodiscard]] std::string GetVendor() const {
            return m_Vendor;
        }

        [[nodiscard]] std::string GetType() const {
            return m_Type;
        }

        [[nodiscard]] bool GetCrfSupport() const {
            return m_CrfSupport;
        }

        [[nodiscard]] bool GetCqSupport() const {
            return m_CqSupport;
        }

        [[nodiscard]] bool GetVideoToolboxCqSupport() const {
            return m_VideoToolboxCqSupport;
        }

        [[nodiscard]] bool GetThreadsLimitSupport() const {
            return m_ThreadsLimitSupported;
        }

        [[nodiscard]] std::vector<std::string>& GetParams() {
            return m_Params;
        }

        [[nodiscard]] std::vector<std::string>& GetThreadsLimitParams() {
            return m_ThreadsLimitParams;
        }

        [[nodiscard]] bool IsAvailable() const {
            return m_Available;
        }

        void SetAvailable(bool available) {
            m_Available = available;
        }
    };
}

#endif //ENCODER_H
