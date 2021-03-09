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
#include "TrailRenderer.h"
#include "PhaseFlow.h"
#include "DifferentialEquation.h"

std::pair<size_t, size_t> ReadWindowSize(std::string fileName = "windowConfig.txt");

int main(void)
{
	ShowWindow(GetConsoleWindow(), SW_SHOW);
	//ShowWindow(GetConsoleWindow(), SW_HIDE);
	std::pair<size_t, size_t> windowSize = ReadWindowSize();
	Engine engine = Engine(windowSize.first, windowSize.second);

	SceneObject* phaseFlowObj = engine.GetScene()->CreateObject("PhaseFlow");
	PhaseFlow* phaseFlow = phaseFlowObj->AddBehaviour<PhaseFlow>();

	
	engine.MainLoop();
	ShowWindow(GetConsoleWindow(), SW_SHOW);
}

std::pair<size_t, size_t> ReadWindowSize(std::string fileName)
{
	std::fstream file(fileName);
	
	size_t width = 0;
	size_t height = 0;

	if (file.is_open() && file.good())
		if (!(file >> width >> height))
			width = height = 0;
	
	file.close();
	if (width < 800)
		width = 800;
	if (height < 600)
		height = 600;

	return std::make_pair(width, height);
}

