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
#include "PhasePortrait.h"
#include "DifferentialEquation.h"

std::tuple<size_t, size_t, bool> ReadWindowSize(std::string fileName = "windowConfig.txt");

int main(void)
{
	ShowWindow(GetConsoleWindow(), SW_SHOW);
	//ShowWindow(GetConsoleWindow(), SW_HIDE);
	std::tuple<size_t, size_t, bool> windowSize = ReadWindowSize("windowConfig.txt");
	Engine engine = Engine(std::get<0>(windowSize), std::get<1>(windowSize), std::get<2>(windowSize));

	SceneObject* phaseFlowObj = engine.GetScene()->CreateObject("PhaseFlow");
	PhasePortrait* phaseFlow = phaseFlowObj->AddBehaviour<PhasePortrait>();

	
	engine.MainLoop();
	ShowWindow(GetConsoleWindow(), SW_SHOW);
}

std::tuple<size_t, size_t,bool> ReadWindowSize(std::string fileName)
{
	std::fstream file(fileName);
	
	size_t width = 0;
	size_t height = 0;
	bool fullScreen = false;

	if (file.is_open() && file.good())
		if (!(file >> width >> height >> fullScreen))
			width = height = 0;
	
	file.close();
	if (width < 800)
		width = 800;
	if (height < 600)
		height = 600;

	return { width,height,fullScreen };
}

