#pragma once

#include "Utils/Utils.h"
#include <sstream>
#include <fstream>
#include <unordered_map>

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath);

    void activate();

	void setUniform(const std::string& name, int value) const;
	void setUniform(const std::string& name, float value) const;
	void setUniform(const std::string& name, bool value) const;
	void setUniform(const std::string& name, const glm::vec3& vec) const;
	void setUniform(const std::string& name, const glm::vec4& vec) const;
    void setUniform(const std::string& name, const glm::mat3& matrix) const;
    void setUniform(const std::string& name, const glm::mat4& matrix) const;

private:
    void checkCompileErrors(unsigned int shader, std::string type);
};