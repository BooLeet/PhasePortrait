// Include standard headers
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <windows.h>

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
#include "Engine/SphericalCoordinateBehaviour.h"
#include "TrailRenderer.h"
#include "PhaseFlow.h"
#include "DifferentialEquation.h"


int main(void)
{
	Engine engine = Engine(1600, 900);
	SceneObject* camera = engine.GetScene()->CreateObject("Camera");
	camera->AddBehaviour<CameraBehaviour>();
	camera->AddBehaviour<SphericalCoordinateBehaviour>()->distance = 12;
	camera->transform.SetPosition(vec3(0, 0, 0));

	SceneObject* phaseFlowObj = engine.GetScene()->CreateObject("PhaseFlow");
	PhaseFlow* phaseFlow = phaseFlowObj->AddBehaviour<PhaseFlow>();

	//ShowWindow(GetConsoleWindow(), SW_HIDE); 
	engine.MainLoop();
	//ShowWindow(GetConsoleWindow(), SW_SHOW);
}

