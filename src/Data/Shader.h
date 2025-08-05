#ifndef SHADER_H
#define SHADER_H
#include <pch.h>

class Shader {
    std::string m_Name;
    std::string m_Path;

public:
    Shader(const std::string& m_name, const std::string& m_path)
        : m_Name(m_name),
          m_Path(m_path) {
    }
};

#endif //SHADER_H
