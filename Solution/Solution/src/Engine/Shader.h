#pragma once
#include "OpenGLComponents.h"
#include <string>

class Shader
{
	GLuint programID;
	bool isValid;

public:
	Shader() : isValid(false), programID(-1){}
	Shader(std::string vertexFilePath, std::string fragmentFilePath);
	~Shader();
	GLuint GetProgram() const;
	bool IsValid() const { return isValid; }
};