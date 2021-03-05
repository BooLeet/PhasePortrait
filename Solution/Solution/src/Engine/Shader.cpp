#include "Shader.h"
#include "ShaderLoader.h"

Shader::Shader(std::string vertexFilePath, std::string fragmentFilePath)
{
	programID = LoadShaders(vertexFilePath.c_str(), fragmentFilePath.c_str());
	isValid = true;
}

Shader::~Shader()
{
	glDeleteProgram(programID);
}

GLuint Shader::GetProgram() const
{
	return programID;
}
