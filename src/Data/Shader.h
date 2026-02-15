#ifndef SHADER_H
#define SHADER_H
#include <pch.h>
#include <utility>

namespace Upscaler {
    class Shader {
    public:
        std::string Name;
        std::string Path;

      Shader(std::string name, std::string path)
            : Name(std::move(name)), Path(std::move(path)) {}
    };
}


#endif //SHADER_H
