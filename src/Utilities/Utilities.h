#ifndef UTILITIES_H
#define UTILITIES_H
#include <pch.h>

#include "Data/UpscalingStatus.hpp"

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

        static std::string ToMegabytes(uintmax_t bytes) {
            double bytesPerMb = 1024.0 * 1024.0;
            double bytesPerGb = 1024.0 * 1024.0 * 1024.0;

            std::ostringstream oss;
            if (bytes >= static_cast<uintmax_t>(bytesPerGb)) {
                double gb = bytes / bytesPerGb;
                oss << std::fixed << std::setprecision(2) << gb << " GB";
                return oss.str();
            }

            double mb = bytes / bytesPerMb;
            oss << std::fixed << std::setprecision(0) << mb << " MB";
            return oss.str();
        }

        static std::string AddUpscaledSuffix(std::string& pathStr) {
            namespace fs = std::filesystem;

            fs::path path(pathStr);
            fs::path parent = path.parent_path();
            std::string stem = path.stem().string();
            std::string ext = path.extension().string();

            fs::path newPath = parent / (stem + "_upscaled" + ext);
            return newPath.string();
        }


       template<typename T>
       static std::string ReplaceAll(std::string str, const std::string& from, const T& value) {
            std::ostringstream oss;
            oss << value;
            std::string to = oss.str();
            size_t pos = 0;

            while ((pos = str.find(from, pos)) != std::string::npos) {
                str.replace(pos, from.length(), to);
                pos += to.length();
            }

            return str;
        }

        static std::string FormatStatus(int upscalingStatus) {
            switch (upscalingStatus) {
                case 0:
                    return "Not started";
                case 1:
                    return "Waiting";
                case 2:
                    return "Processing";
                case 3:
                    return "Failed";
                case 4:
                    return "Finished";
                default:
                     return "Unknown";
            }
        }


        static void PreventSleep(Logger& logger) {
#ifdef _WIN32
            Windows::PreventSleep(logger);
#else
            logger.Warn("Sleep prevention is not supported on your operating system. Please ensure your computer does not go to sleep while processing videos.");
#endif

        }
    };
}
#endif //UTILITIES_H
