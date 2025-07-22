#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/spdlog.h>

#include "spdlog/sinks/stdout_color_sinks.h"

#define LOG_INFO(message, ...) Upscaler::Logger::Info(message, ##__VA_ARGS__)
#define LOG_WARN(message, ...) Upscaler::Logger::Warn(message, ##__VA_ARGS__)
#define LOG_ERROR(message, ...) Upscaler::Logger::Error(message, ##__VA_ARGS__)
#define LOG_CRITICAL(message, ...) Upscaler::Logger::Critical(message, ##__VA_ARGS__)
#define LOG_TRACE(message, ...) Upscaler::Logger::Trace(message, ##__VA_ARGS__)
#define LOG_DEBUG(message, ...) Upscaler::Logger::Debug(message, ##__VA_ARGS__)
#define ASSERT(condition, message, ...) \
    Upscaler::Logger::Assert(condition, message, ##__VA_ARGS__)

namespace Upscaler {
class Logger {
public:
    static void Init() {
        spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
        spdlog::set_default_logger(spdlog::stdout_color_mt("CFLogger"));
#if defined(DEBUG) || defined(_DEBUG)
        spdlog::set_level(spdlog::level::trace);
#else
        spdlog::set_level(spdlog::level::info);
#endif
    }

    static void Shutdown() {
        spdlog::shutdown();
    }

    template <typename... Args>
    static void Info(fmt::format_string<Args...> message, Args&&... args) {
        spdlog::info(message, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void Warn(fmt::format_string<Args...> message, Args&&... args) {
        spdlog::warn(message, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void Error(fmt::format_string<Args...> message, Args&&... args) {
        spdlog::error(message, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void Critical(fmt::format_string<Args...> message, Args&&... args) {
        spdlog::critical(message, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void Trace(fmt::format_string<Args...> message, Args&&... args) {
        spdlog::trace(message, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void Debug(fmt::format_string<Args...> message, Args&&... args) {
        spdlog::debug(message, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void Assert(const bool condition, const std::string& message, Args&&... args) {
        if (!condition) {
            spdlog::critical("Assertion failed: {}", message, std::forward<Args>(args)...);
            throw std::runtime_error(message);
        }
    }

    static void SetLevel(spdlog::level::level_enum level) {
        spdlog::set_level(level);
    }

    static spdlog::level::level_enum GetLevel() {
        return spdlog::get_level();
    }

    static std::shared_ptr<spdlog::logger> GetDefaultLogger() {
        return spdlog::default_logger();
    }
};

} // namespace Upscaler

#endif // LOGGER_H