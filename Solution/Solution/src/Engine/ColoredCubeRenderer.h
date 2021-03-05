#pragma once
#include "EngineLibrary.h"

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

class Mesh;

class ColoredCubeRenderer : public RendererBehaviour
{
	Mesh* meshToDraw;

public:
	Shader* shader;
	
	void Start();

	void Render(mat4 projectionViewMatrix);

	void Update();

	void OnDestroyRenderer();
};