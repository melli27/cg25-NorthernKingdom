#pragma once

#include "Utils/Utils.h"
#include <sstream>
#include <fstream>
#include <unordered_map>

class Shader {
private:
    GLuint shader;
    std::unordered_map<std::string, GLint> uniformLocationMap;

public:
    Shader();
    ~Shader();

    void createShader();

    GLuint compileShader(GLuint type, const std::string& path) const;
    void activate();

    void setUniform(const std::string& name, const glm::mat4& mat);
    void setUniform(const std::string& name, const glm::vec3& vec3);

    GLint getUniformLocation(const std::string& name);

};