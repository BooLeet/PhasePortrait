#include "Engine.h"
#include "Scene.h"
#include "RendererBehaviour.h"
#include "CameraBehaviour.h"
#include "Input.h"

#include <algorithm>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
using namespace glm;
//#include "EngineLibrary.h"

#include <iostream>

// Time Counter
#include <chrono> 
using namespace std::chrono;

Engine::Engine(size_t windowWidth, size_t windowHeight)
{
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
	timeScale = 1;
	deltaTime = 0;
	unscaledDeltaTime = 0;
	scene = new Scene(this);
	input = nullptr;
	window = nullptr;
}

Engine::~Engine()
{
	delete scene;
	delete input;
}

int Engine::MainLoop()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(windowWidth, windowHeight, "Window", NULL, NULL);
	input = new Input(window);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);


	do 
	{
		auto start = high_resolution_clock::now();
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		scene->UpdateAllObjects();
		cameraRegistry.RenderCameras(rendererRegistry);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>(stop - start);
		deltaTime = duration.count() * 0.000001;
	} while (!CloseWindow());

	// Cleanup VBO and shader
	/*glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);*/

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

Scene* Engine::GetScene()
{
	return scene;
}

size_t Engine::GetWindowWidth() const
{
	return windowWidth;
}

size_t Engine::GetWindowHeight() const
{
	return windowHeight;
}

double Engine::GetDeltaTime() const
{
	return deltaTime * timeScale;
}

double Engine::GetUnscaledDeltaTime() const
{
	return unscaledDeltaTime;
}

Input* Engine::GetInput() const
{
	return input;
}

bool Engine::CloseWindow()
{
	return glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
		glfwWindowShouldClose(window) == 1;
}

void Engine::CameraRegistry::RegisterCamera(CameraBehaviour* cam)
{
	auto iterator = std::find(cameras.begin(), cameras.end(), cam);
	if (iterator != cameras.end())
		return;

	cameras.push_back(cam);
}

void Engine::CameraRegistry::UnregisterCamera(CameraBehaviour* cam)
{
	auto iterator = std::find(cameras.begin(), cameras.end(), cam);
	if (iterator == cameras.end())
		return;
	std::iter_swap(iterator, cameras.end() - 1);
	cameras.pop_back();
}

void Engine::CameraRegistry::RenderCameras(const Engine::RendererRegistry& rendererRegistry) const
{
	for (CameraBehaviour* camera : cameras)
	{
		mat4 projectionView = camera->GetProjectionViewMatrix();
		for (size_t i = 0; i < rendererRegistry.Size(); ++i)
			rendererRegistry[i]->Render(projectionView);
	}
}

void Engine::RendererRegistry::RegisterRenderer(RendererBehaviour* renderer)
{
	std::cout << "RegisterRendererStart\n";
	auto iterator = std::find(renderers.begin(), renderers.end(), renderer);
	if (iterator != renderers.end())
		return;

	renderers.push_back(renderer);
	std::cout << "RegisterRendererEnd\n";
}

void Engine::RendererRegistry::UnregisterRenderer(RendererBehaviour* renderer)
{
	auto iterator = std::find(renderers.begin(), renderers.end(), renderer);
	if (iterator == renderers.end())
		return;
	std::iter_swap(iterator, renderers.end() - 1);
	renderers.pop_back();
}

void Engine::RendererRegistry::RenderAll(CameraBehaviour* camera) const
{

}

RendererBehaviour* Engine::RendererRegistry::operator[](size_t index) const
{
	return renderers[index];
}

size_t Engine::RendererRegistry::Size() const
{
	return renderers.size();
}
