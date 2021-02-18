#pragma once
#include "RendererBehaviour.h"
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

class Mesh;

class ColoredCubeRenderer : public RendererBehaviour
{
	Mesh* meshToDraw;

public:
	GLuint programID = 0;
	
	void Start();

	void Render(mat4 projectionViewMatrix);

	void Update();

	void OnDestroyRenderer();
};