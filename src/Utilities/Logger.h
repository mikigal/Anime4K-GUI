#ifndef LOGGER_H
#define LOGGER_H

#include <pch.h>
#include <spdlog/spdlog.h>

#include "spdlog/sinks/stdout_color_sinks.h"

namespace Upscaler {
    class Logger {
    public:
        void Init(bool debug) {
            spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
            spdlog::set_default_logger(spdlog::stdout_color_mt("UpscalerLogger"));
            if (debug) {

            }
            spdlog::set_level(debug ? spdlog::level::debug : spdlog::level::info);
        }

        void Shutdown() {
            spdlog::shutdown();
        }

        template<typename... Args>
        void Info(fmt::format_string<Args...> message, Args&&... args) {
            spdlog::info(message, std::forward<Args>(args)...);
            if (spdlog::should_log(spdlog::level::info)) {
                std::string formattedMessage = fmt::format(message, std::forward<Args>(args)...);
                Renderer::Logs += fmt::format("[{}] [info] {}\n", GetFormattedTime(), formattedMessage);
            }
        }

        template<typename... Args>
        void Warn(fmt::format_string<Args...> message, Args&&... args) {
            spdlog::warn(message, std::forward<Args>(args)...);
            if (spdlog::should_log(spdlog::level::warn)) {
                std::string formattedMessage = fmt::format(message, std::forward<Args>(args)...);
                Renderer::Logs += fmt::format("[{}] [warn] {}\n", GetFormattedTime(), formattedMessage);
            }
        }

        template<typename... Args>
        void Error(fmt::format_string<Args...> message, Args&&... args) {
            spdlog::error(message, std::forward<Args>(args)...);
            if (spdlog::should_log(spdlog::level::err)) {
                std::string formattedMessage = fmt::format(message, std::forward<Args>(args)...);
                Renderer::Logs += fmt::format("[{}] [error] {}\n", GetFormattedTime(), formattedMessage);
            }
        }

        template<typename... Args>
        void Critical(fmt::format_string<Args...> message, Args&&... args) {
            spdlog::critical(message, std::forward<Args>(args)...);
            if (spdlog::should_log(spdlog::level::critical)) {
                std::string formattedMessage = fmt::format(message, std::forward<Args>(args)...);
                Renderer::Logs += fmt::format("[{}] [critical] {}\n", GetFormattedTime(), formattedMessage);
            }
        }

        template<typename... Args>
        void Trace(fmt::format_string<Args...> message, Args&&... args) {
            spdlog::trace(message, std::forward<Args>(args)...);
            if (spdlog::should_log(spdlog::level::trace)) {
                std::string formattedMessage = fmt::format(message, std::forward<Args>(args)...);
                Renderer::Logs += fmt::format("[{}] [trace] {}\n", GetFormattedTime(), formattedMessage);
            }
        }

        template<typename... Args>
        void Debug(fmt::format_string<Args...> message, Args&&... args) {
            spdlog::debug(message, std::forward<Args>(args)...);
            if (spdlog::should_log(spdlog::level::debug)) {
                std::string formattedMessage = fmt::format(message, std::forward<Args>(args)...);
                Renderer::Logs += fmt::format("[{}] [debug] {}\n", GetFormattedTime(), formattedMessage);
            }
        }

        template<typename... Args>
        void Assert(const bool condition, const std::string& message, Args&&... args) {
            if (!condition) {
                spdlog::critical("Assertion failed: {}", message, std::forward<Args>(args)...);
                throw std::runtime_error(message);
            }
        }

        void SetLevel(spdlog::level::level_enum level) {
            spdlog::set_level(level);
        }

        spdlog::level::level_enum GetLevel() {
            return spdlog::get_level();
        }

        std::shared_ptr<spdlog::logger> GetDefaultLogger() {
            return spdlog::default_logger();
        }

        std::string GetFormattedTime() {
            auto now = std::chrono::system_clock::now();
            std::time_t now_c = std::chrono::system_clock::to_time_t(now);
            std::tm tm = *std::localtime(&now_c);

            char timeBuffer[9]; // "HH:MM:SS"
            std::strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", &tm);
            return std::string(timeBuffer);
        }
    };
} // namespace Upscaler

#endif // LOGGER_H
