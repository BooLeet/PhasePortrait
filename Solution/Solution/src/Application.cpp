// Include standard headers
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

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

int main(void)
{
	Engine engine = Engine(1280, 720);
	SceneObject* camera = engine.GetScene()->CreateObject("Camera");
	camera->AddBehaviour<CameraBehaviour>();
	camera->AddBehaviour<SphericalCoordinateBehaviour>()->distance = 10;
	camera->transform.SetPosition(vec3(0, 0, 0));

	/*SceneObject* coloredCube = engine.GetScene()->CreateObject("ColoredCube");
	coloredCube->AddBehaviour<ColoredCubeRenderer>();*/
	//coloredCube->transform.SetScale(vec3(50, 50, 50));

	/*SceneObject* trailTest = engine.GetScene()->CreateObject("Trail");
	TrailRenderer* trailRenderer = trailTest->AddBehaviour<TrailRenderer>();
	trailRenderer->renderMode = TrailRenderer::RenderMode::Lines;*/
	
	SceneObject* phaseFlowObj = engine.GetScene()->CreateObject("PhaseFlow");
	PhaseFlow* phaseFlow = phaseFlowObj->AddBehaviour<PhaseFlow>();
	phaseFlow->renderMode = TrailRenderer::RenderMode::Lines;
	
	std::string fileName = "Pend.txt";
	std::ifstream file(fileName);
	phaseFlow->rightSideExpression = ReadExpression(file);

	phaseFlow->sampleSize = 30;
	//phaseFlow->differentialEquationOrder = 3;
	
	engine.timeScale = 1;

	engine.MainLoop();
}

