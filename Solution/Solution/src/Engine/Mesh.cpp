#include "Mesh.h"
#include "Transform.h"
#include <assert.h>
#include <iostream>

Mesh::Mesh(const std::vector<GLfloat>& vertexData, const std::vector<GLfloat>& colorData, GLuint programID, GLint drawMode)
{
	assert(vertexData.size() == colorData.size());

	bufferSize = vertexData.size();

	this->drawMode = drawMode;


	shaderProgramID = programID;
	MatrixID = glGetUniformLocation(shaderProgramID, "MVP");

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * bufferSize, &vertexData[0], GL_STATIC_DRAW);

	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * bufferSize, &colorData[0], GL_STATIC_DRAW);
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &colorBuffer);
}

void Mesh::Render(const mat4& projectionViewMatrix,const mat4& modelMatrix)
{
	glUseProgram(shaderProgramID);

	glm::mat4 MVP = projectionViewMatrix * modelMatrix;
	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	glDrawArrays(drawMode, 0, bufferSize / 3);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

