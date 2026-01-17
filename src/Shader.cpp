#include "Shader.h"
#include <iostream>

#include <filesystem>

using namespace std;


void Shader::createSimpleShader()
{
	shader = glCreateProgram();
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, "src/Shader/vertex.shader");
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, "src/Shader/fragment.shader");

	glAttachShader(shader, vertexShader);
	glAttachShader(shader, fragmentShader);
	glLinkProgram(shader);
	glValidateProgram(shader);

	ID = shader;

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::createNormalShader()
{
	shader = glCreateProgram();
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, "src/Shader/normal_vert.shader");
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, "src/Shader/normal_frag.shader");

	glAttachShader(shader, vertexShader);
	glAttachShader(shader, fragmentShader);
	glLinkProgram(shader);
	glValidateProgram(shader);

	ID = shader;

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::createLightingShader()
{
	shader = glCreateProgram();
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, "src/Shader/lighting_vert.shader");
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, "src/Shader/lighting_frag.shader");

	glAttachShader(shader, vertexShader);
	glAttachShader(shader, fragmentShader);
	glLinkProgram(shader);
	glValidateProgram(shader);

	ID = shader;

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::createLightSourceShader()
{
	shader = glCreateProgram();
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, "src/Shader/light_source_vert.shader");
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, "src/Shader/light_source_frag.shader");

	glAttachShader(shader, vertexShader);
	glAttachShader(shader, fragmentShader);
	glLinkProgram(shader);
	glValidateProgram(shader);

	ID = shader;

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::createDepthShader()
{
	shader = glCreateProgram();
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, "src/Shader/depth_vert.shader");
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, "src/Shader/depth_frag.shader");

	glAttachShader(shader, vertexShader);
	glAttachShader(shader, fragmentShader);
	glLinkProgram(shader);
	glValidateProgram(shader);

	ID = shader;

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::createPointDepthShader()
{
	shader = glCreateProgram();
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, "src/Shader/point_depth_vert.shader");
	GLuint geometryShader = compileShader(GL_GEOMETRY_SHADER, "src/Shader/point_depth_geom.shader");
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, "src/Shader/point_depth_frag.shader");

	glAttachShader(shader, vertexShader);
	glAttachShader(shader, geometryShader);
	glAttachShader(shader, fragmentShader);
	glLinkProgram(shader);
	glValidateProgram(shader);

	ID = shader;

	glDeleteShader(vertexShader);
	glDeleteShader(geometryShader);
	glDeleteShader(fragmentShader);
}

void Shader::createTerrainShader()
{
	shader = glCreateProgram();
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, "src/Shader/terrain_vert.shader");
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, "src/Shader/terrain_frag.shader");
	unsigned int tessControlShader = compileShader(GL_TESS_CONTROL_SHADER, "src/Shader/terrain_tcs.shader");
	unsigned int tessEvalShader = compileShader(GL_TESS_EVALUATION_SHADER, "src/Shader/terrain_tes.shader");

	glAttachShader(shader, vertexShader);
	glAttachShader(shader, fragmentShader);
	glAttachShader(shader, tessControlShader);
	glAttachShader(shader, tessEvalShader);
	glLinkProgram(shader);
	glValidateProgram(shader);

	ID = shader;

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(tessControlShader);
	glDeleteShader(tessEvalShader);
}

void Shader::createAnimatedModelShader()
{
	shader = glCreateProgram();
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, "src/Shader/animation_vert.shader");
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, "src/Shader/animation_frag.shader");

	glAttachShader(shader, vertexShader);
	glAttachShader(shader, fragmentShader);
	glLinkProgram(shader);
	glValidateProgram(shader);

	ID = shader;

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::activate() {
	glUseProgram(shader);
}

Shader::Shader() {}

Shader::~Shader() {
	glDeleteProgram(shader);
}

GLuint Shader::compileShader(GLuint type, const std::string& path) const {
	std::ifstream stream(path);
	std::stringstream buffer;
	buffer << stream.rdbuf();
	std::string source = buffer.str();

	GLuint shaderId = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(shaderId, 1, &src, nullptr);
	glCompileShader(shaderId);

	GLint succeeded;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &succeeded);

	if (succeeded == GL_FALSE) {
		//Log auslesen und ausgeben
		GLint logSize;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logSize);
		char* message = new char[logSize];
		glGetShaderInfoLog(shaderId, logSize, nullptr, message);
		std::cerr << "Error in compileShader for " << path << ": " << message << std::endl;
		delete[] message;
	}

	return shaderId;
}

GLint Shader::getUniformLocation(const std::string& name) {
	if (uniformLocationMap.find(name) != uniformLocationMap.end()) {
		return uniformLocationMap[name];
	}
	GLint id = glGetUniformLocation(shader, name.c_str());
	uniformLocationMap[name] = id;
	return id;
}

void Shader::setUniform(const std::string& name, const glm::mat4& matrix)
{
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

void Shader::setUniform(const std::string& name, const glm::vec3& vec)
{
	glUniform3f(getUniformLocation(name), vec[0], vec[1], vec[2]);
}

void Shader::setUniform(const std::string& name, float v)
{
	glUniform1f(getUniformLocation(name), v);
}

void Shader::setUniform(const string& name, int v)
{
	glUniform1i(getUniformLocation(name), v);
}

void Shader::setUniformMatrix4fv(const std::string& name, int size, GLboolean transposed, glm::mat4 matrix) {
	setUniformMatrix4fv(name, size, transposed, &matrix[0][0]);
}

void Shader::setUniformMatrix4fv(const std::string& name, int size, GLboolean transposed, GLfloat* matrix) {
	glUniformMatrix4fv(getUniformLocation(name), size, transposed, matrix);
}
