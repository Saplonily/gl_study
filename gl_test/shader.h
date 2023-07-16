#pragma once

#ifndef SHADER_H
#define SHADER_H

#include <glm/matrix.hpp>
#include <glm/vector_relational.hpp>
#include <string>

class Shader
{
public:
	unsigned int ID;

	static Shader* fromFile(const char* vertexPath, const char* fragmentPath);
	static std::shared_ptr<Shader> sptrFromFile(const char* vertexPath, const char* fragmentPath);

	~Shader();
	Shader(const char* vertexSourceCode, const char* fragmentSourceCode);
	Shader(Shader&& other) noexcept;

	void use() const;
	GLint getLocation(const std::string& name) const;

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMat3(const std::string& name, const glm::mat3& value) const;
	void setMat4(const std::string& name, const glm::mat4& value) const;
	void setVec3(const std::string& name, glm::vec3 value) const;
	void setVec4(const std::string& name, glm::vec4 value) const;

	void setBool(int location, bool value) const;
	void setInt(int location, int value) const;
	void setFloat(int location, float value) const;
	void setMat3(int location, const glm::mat3& value) const;
	void setMat4(int location, const glm::mat4& value) const;
	void setVec3(int location, glm::vec3 value) const;
	void setVec4(int location, glm::vec4 value) const;

private:
	void checkCompileErrors(unsigned int shader, const std::string& type);
};
#endif