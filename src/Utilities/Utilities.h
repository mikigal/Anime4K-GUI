#ifndef UTILITIES_H
#define UTILITIES_H
#include <pch.h>

#ifdef _WIN32
#include "WindowsUtilities.h"
#endif

namespace Upscaler {
    class Utilities {
    public:

        static std::vector<std::string> Split(const std::string& input, const std::string& delimiter) {
            std::vector<std::string> tokens;
            size_t start = 0;
            size_t end = 0;

            while ((end = input.find(delimiter, start)) != std::string::npos) {
                tokens.push_back(input.substr(start, end - start));
                start = end + delimiter.length();
            }

            // Ostatni fragment po ostatnim delimiterze
            tokens.push_back(input.substr(start));

            return tokens;
        }

        static std::string FormatTime(int totalSeconds) {
            int hours = totalSeconds / 3600;
            int minutes = (totalSeconds % 3600) / 60;
            int seconds = totalSeconds % 60;

            std::ostringstream oss;
            if (hours > 0) {
                oss << std::setw(2) << std::setfill('0') << hours << ":"
                    << std::setw(2) << std::setfill('0') << minutes << ":"
                    << std::setw(2) << std::setfill('0') << seconds;
                return oss.str();
            }

            oss << std::setw(2) << std::setfill('0') << minutes << ":"
                    << std::setw(2) << std::setfill('0') << seconds;
            return oss.str();
        }

        static int ToMegabytes(long bytes) {
            return bytes / 1024 / 1024;
        }

        static void PreventSleep(Logger& logger) {
#ifdef _WIN32
            Windows::PreventSleep(logger);
#else
            logger.Warn("Sleep prevention is not supported on this operating system. Please ensure your computer does not go to sleep while processing videos.");
#endif

        }
    };
}
#endif //UTILITIES_H
