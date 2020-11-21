#pragma once
#include "RendererBehaviour.h"
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

class ColoredCubeRenderer : public RendererBehaviour
{
public:
	GLuint VertexArrayID = 0;
	GLuint programID = 0;
	GLuint MatrixID = 0;
	GLuint vertexbuffer = 0;
	GLuint colorbuffer = 0;

	void Start();

	void Render(mat4 projectionViewMatrix);

	void Update();

	void OnDestroy();
};