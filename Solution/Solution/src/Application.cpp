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
	Engine engine = Engine(1280, 720);
	SceneObject* camera = engine.GetScene()->CreateObject("Camera");
	camera->AddBehaviour<CameraBehaviour>();
	camera->AddBehaviour<SphericalCoordinateBehaviour>()->distance = 12;
	camera->transform.SetPosition(vec3(0, 0, 0));

	/*SceneObject* coloredCube = engine.GetScene()->CreateObject("ColoredCube");
	coloredCube->AddBehaviour<ColoredCubeRenderer>();*/
	//coloredCube->transform.SetScale(vec3(50, 50, 50));

	/*SceneObject* coordinateSystem = engine.GetScene()->CreateObject("Coordinate System");
	CoordinateSystemRenderer* coordinateRenderer = coordinateSystem->AddBehaviour<CoordinateSystemRenderer>();
	coordinateRenderer->SetRadius(20);
	coordinateRenderer->SetSegmentLength(1.5707963);*/
	
	SceneObject* phaseFlowObj = engine.GetScene()->CreateObject("PhaseFlow");

	/*CoordinateSystemRenderer* coordinateRenderer = phaseFlowObj->AddBehaviour<CoordinateSystemRenderer>();
	coordinateRenderer->SetRadius(20);
	coordinateRenderer->SetSegmentLength(1.5707963);*/

	PhaseFlow* phaseFlow = phaseFlowObj->AddBehaviour<PhaseFlow>();
	phaseFlow->renderMode = TrailRenderer::RenderMode::Lines;

	
	
	/*std::string fileName;
	std::cout << "Enter the file name: ";
	std::cin >> fileName;

	std::ifstream file(fileName);

	DifferentialEquation differentialEquation(2, Expression::ReadExpression(file));
	phaseFlow->SetDifferentialEquation(differentialEquation);*/

	phaseFlow->sampleSize = 32;
	//phaseFlow->differentialEquationOrder = 3;
	
	engine.timeScale = 1;

	ShowWindow(GetConsoleWindow(), SW_HIDE);
	engine.MainLoop();
	//ShowWindow(GetConsoleWindow(), SW_SHOW);
}

