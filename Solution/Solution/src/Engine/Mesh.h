#pragma once
#include "OpenGLComponents.h"
#include "Shader.h"
#include <vector>

class Transform;

class Mesh
{
	GLuint vertexBuffer;
	GLuint colorBuffer;

	GLuint shaderProgramID;

	GLuint MatrixID;

	size_t bufferSize;

public:
	GLint drawMode;
	Mesh(const std::vector<GLfloat>& vertexData, const std::vector<GLfloat>& colorData, GLuint programID, GLint drawMode = GL_TRIANGLES);
	~Mesh();

	void Render(const mat4& projectionViewMatrix, const mat4& modelMatrix);
};
