#ifndef UTILITIES_H
#define UTILITIES_H
#include <pch.h>

namespace Upscaler {
    std::vector<std::string> split(const std::string& text, char separator) {
        std::vector<std::string> result;
        std::stringstream ss(text);
        std::string element;

        while (std::getline(ss, element, separator)) {
            if (!element.empty())
                result.push_back(element);
        }

        return result;
    }
}
#endif //UTILITIES_H
