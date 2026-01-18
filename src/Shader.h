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
    GLuint ID;

    Shader();
    ~Shader();

    void createSimpleShader();
    void createNormalShader();
    void createLightingShader();
	void createLightSourceShader();
    void createDepthShader();
    void createPointDepthShader();
	void createSkyboxShader();

    void createTerrainShader();
    void createAnimatedModelShader();

    GLuint compileShader(GLuint type, const std::string& path) const;
    void activate();

    void setUniform(const std::string& name, const glm::mat4& mat);
    void setUniform(const std::string& name, const glm::vec3& vec3);
    void setUniform(const std::string& name, float v);
    void setUniform(const std::string& name, int v);

    void setUniformMatrix4fv(const std::string& name, int size, GLboolean transposed, glm::mat4 matrix);
    void setUniformMatrix4fv(const std::string& name, int size, GLboolean transposed, GLfloat* matrix);

    GLint getUniformLocation(const std::string& name);

};