#ifndef SHADER_H
#define SHADER_H
#include <pch.h>
#include <utility>

namespace Upscaler {
    class Shader {
    private:
        std::string m_Name;
        std::string m_Path;

    public:
      Shader(std::string name, std::string path)
            : m_Name(std::move(name)), m_Path(std::move(path)) {}

        [[nodiscard]] const std::string& GetName() const {
            return m_Name;
        }

        [[nodiscard]] std::string& GetPath() {
            return m_Path;
        }
    };
}


#endif //SHADER_H
