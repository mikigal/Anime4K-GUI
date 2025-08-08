#ifndef UTILITIES_H
#define UTILITIES_H
#include <pch.h>

namespace Upscaler {
    class Utilities {
    public:
        static std::vector<std::string> Split(const std::string& text, char separator) {
            std::vector<std::string> result;
            std::stringstream ss(text);
            std::string element;

            while (std::getline(ss, element, separator)) {
                if (!element.empty())
                    result.push_back(element);
            }

            return result;
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
    };
}
#endif //UTILITIES_H
