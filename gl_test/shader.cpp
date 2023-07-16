#include "pch.h"
#include "shader.h"
#include <string>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>
#include <tuple>

Shader* Shader::fromFile(const char* vertexPath, const char* fragmentPath)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	return new Shader(vShaderCode, fShaderCode);
}

std::shared_ptr<Shader> Shader::sptrFromFile(const char* vertexPath, const char* fragmentPath)
{
	Shader* ptr = Shader::fromFile(vertexPath, fragmentPath);
	return std::shared_ptr<Shader>(ptr);
}

Shader::~Shader()
{
	//std::cout << "shader des" << std::endl;
}

Shader::Shader(const char* vertexSourceCode, const char* fragmentSourceCode)
{
	//std::cout << "shader cons" << std::endl;
	// 2. compile shaders
	unsigned int vertex, fragment;
	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexSourceCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentSourceCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");
	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::Shader(Shader&& other) noexcept
{
	ID = other.ID;
	other.ID = -1;
}

// activate the shader
void Shader::use() const { glUseProgram(ID); }
int Shader::getLocation(const std::string& name) const { return glGetUniformLocation(ID, name.c_str()); }

// utility uniform functions
void Shader::setBool(const std::string& name, bool value) const { setBool(getLocation(name), value); }
void Shader::setInt(const std::string& name, int value) const { setInt(getLocation(name), value); }
void Shader::setFloat(const std::string& name, float value) const { setFloat(getLocation(name), value); }
void Shader::setMat3(const std::string& name, const glm::mat3& value) const { setMat3(getLocation(name), value); }
void Shader::setMat4(const std::string& name, const glm::mat4& value) const { setMat4(getLocation(name), value); }
void Shader::setVec3(const std::string& name, glm::vec3 value) const { setVec3(getLocation(name), value); }
void Shader::setVec4(const std::string& name, glm::vec4 value) const { setVec4(getLocation(name), value); }

void Shader::setBool(int location, bool value) const { glUniform1i(location, (int)value); }
void Shader::setInt(int location, int value) const { glUniform1i(location, value); }
void Shader::setFloat(int location, float value) const { glUniform1f(location, value); }
void Shader::setMat3(int location, const glm::mat3& value) const { glUniformMatrix3fv(location, 1, false, glm::value_ptr(value)); }
void Shader::setMat4(int location, const glm::mat4& value) const { glUniformMatrix4fv(location, 1, false, glm::value_ptr(value)); }
void Shader::setVec3(int location, glm::vec3 value) const { glUniform3f(location, value.x, value.y, value.z); }
void Shader::setVec4(int location, glm::vec4 value) const { glUniform4f(location, value.x, value.y, value.z, value.w); }

// utility function for checking shader compilation/linking errors.
void Shader::checkCompileErrors(unsigned int shader, const std::string& type)
{
	static const std::string& compilationErrorHead = "ERROR::SHADER_COMPILATION_ERROR of type: ";
	static const std::string& linkErrorHead = "ERROR::PROGRAM_LINKING_ERROR of type: ";
	static const std::string& separator = "\n -- --------------------------------------------------- -- ";
	using namespace std::literals;
	int success;
	const int arrLength = 1024;
	char infoLog[arrLength];
	if (type != "PROGRAM"s)
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (success) return;
		glGetShaderInfoLog(shader, arrLength, NULL, infoLog);
		std::cerr << compilationErrorHead << type << "\n" << infoLog << separator << std::endl;
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (success) return;
		glGetProgramInfoLog(shader, arrLength, NULL, infoLog);
		std::cerr << linkErrorHead << type << "\n" << infoLog << separator << std::endl;
	}
}