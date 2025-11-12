#include "Shader.h"
#include <iostream>

#include <filesystem>

using namespace std;


void Shader::createShader()
{
	shader = glCreateProgram();
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, "src/Shader/vertex.shader");
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, "src/Shader/fragment.shader");

	glAttachShader(shader, vertexShader);
	glAttachShader(shader, fragmentShader);
	glLinkProgram(shader);
	glValidateProgram(shader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::activate() {
	glUseProgram(shader);
}

Shader::Shader(){}

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
	activate();
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

void Shader::setUniform(const std::string& name, const glm::vec3& vec)
{
	activate();
	glUniform3f(getUniformLocation(name), vec[0], vec[1], vec[2]);
}
