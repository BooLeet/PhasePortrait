// Include standard headers
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
using namespace glm;

//#include "Engine/ShaderLoader.h"

// Time Counter
#include <chrono> 
using namespace std::chrono;

#include "Engine/Engine.h"
#include "Engine/Scene.h"
#include "Engine/SceneObject.h"
#include "Engine/ColoredCubeRenderer.h"
#include "Engine/CameraBehaviour.h"

int main(void)
{
	Engine engine = Engine(1280, 720);
	SceneObject* camera = engine.GetScene()->CreateObject("Camera");
	camera->AddBehaviour<CameraBehaviour>();
	camera->transform.SetPosition(vec3(0, 0, 0));

	SceneObject* coloredCube = engine.GetScene()->CreateObject("ColoredCube");
	coloredCube->AddBehaviour<ColoredCubeRenderer>();
	coloredCube->transform.SetRotation(vec3(3.14/4, 0, 0));
	coloredCube->transform.SetPosition(vec3(2,1,-5));//-1, -3, 5

	SceneObject* coloredCube1 = engine.GetScene()->CreateObject("ColoredCube");
	coloredCube1->AddBehaviour<ColoredCubeRenderer>();
	coloredCube1->transform.SetRotation(vec3(3.14 / 4, 0, 0));
	coloredCube1->transform.SetPosition(vec3(-2, -1, -5));//-1, -3, 5

	engine.MainLoop();
}

