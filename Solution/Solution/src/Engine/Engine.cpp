#include "Engine.h"
#include "Scene.h"
#include "RendererBehaviour.h"
#include "CameraBehaviour.h"
#include "Input.h"
#include "Shader.h"

#include <algorithm>
#include <iostream>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw_gl3.h"

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



#include "ShaderLoader.h"

Engine::Engine(size_t windowWidth, size_t windowHeight)
{
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
	timeScale = 1;
	unscaledDeltaTime = 0;
	scene = new Scene(this);
	input = nullptr;
	window = nullptr;
}

Engine::~Engine()
{
	delete scene;
	delete input;
	delete defaultShader;
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

	// Setup ImGui binding
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
	ImGui_ImplGlfwGL3_Init(window, true);

	// Setup style
	//ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();
	//ImGui::StyleColorsLight();

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID = 0;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);


	// default shader setup
	defaultShader = new Shader("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");

	do 
	{
		auto start = high_resolution_clock::now();
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplGlfwGL3_NewFrame();

		scene->UpdateAllObjects();
		cameraRegistry.RenderCameras(rendererRegistry);

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();


		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>(stop - start);
		unscaledDeltaTime = duration.count() * 0.000001;

		// To prevent bugs caused by big lags
		if (unscaledDeltaTime >= 0.05)
			unscaledDeltaTime = 0.05;
	} while (!CloseWindow());

	glDeleteVertexArrays(1, &VertexArrayID);

	// Cleanup
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
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
	return unscaledDeltaTime * timeScale;
}

double Engine::GetUnscaledDeltaTime() const
{
	return unscaledDeltaTime;
}

Input* Engine::GetInput() const
{
	return input;
}

void Engine::ConsoleLog(std::string str) const
{
	std::cout << str << '\n';
}

const Shader& Engine::GetDefaultShader() const
{
	return *defaultShader;
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
		rendererRegistry.RenderAll(camera);
	}
}

void Engine::RendererRegistry::RegisterRenderer(RendererBehaviour* renderer)
{
	auto iterator = std::find(renderers.begin(), renderers.end(), renderer);
	if (iterator != renderers.end())
		return;

	renderers.push_back(renderer);
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
	mat4 projectionView = camera->GetProjectionViewMatrix();
	for (size_t i = 0; i < Size(); ++i)
		(*this)[i]->TryRender(projectionView);
}

RendererBehaviour* Engine::RendererRegistry::operator[](size_t index) const
{
	return renderers[index];
}

size_t Engine::RendererRegistry::Size() const
{
	return renderers.size();
}
