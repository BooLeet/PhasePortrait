#pragma once
#include <vector>
#include "OpenGLComponents.h"

#include <string>

class Scene;

class CameraBehaviour;

class RendererBehaviour;

class Input;

class Shader;

class Engine
{
private:
	GLFWwindow* window;
	Scene* scene;
	
	double unscaledDeltaTime;
	size_t windowWidth, windowHeight;
	bool fullScreen;
	Input* input;
	Shader* defaultShader;
	bool quitFlag = false;

	bool CloseWindow();
public:
	vec3 clearColor = vec3(0, 0, 0);
	double timeScale;

	// Has references for all the renderers on the scene
	class RendererRegistry
	{
	private:
		std::vector< RendererBehaviour*> renderers;
	public:
		// Adds a renderer
		void RegisterRenderer(RendererBehaviour* renderer);
		// Removes a renderer
		void UnregisterRenderer(RendererBehaviour* renderer);
		// Renders all objects
		void RenderAll(CameraBehaviour* camera) const;
		RendererBehaviour* operator[](size_t index) const;
		size_t Size() const;
	};

	// Has references for all the cameras on the scene
	class CameraRegistry
	{
	private:
		std::vector<CameraBehaviour*> cameras;
	public:
		// Adds a camera
		void RegisterCamera(CameraBehaviour* cam);
		// Finds and removes a camera
		void UnregisterCamera(CameraBehaviour* cam);

		void RenderCameras(const RendererRegistry& rendererRegistry) const;
	};

	CameraRegistry cameraRegistry;
	RendererRegistry rendererRegistry;

	Engine(size_t windowWidth, size_t windowHeight,bool fullscreen);
	~Engine();
	// Main engine loop
	int MainLoop();

	Scene* GetScene();

	size_t GetWindowWidth() const;
	size_t GetWindowHeight() const;
	double GetDeltaTime() const;
	double GetUnscaledDeltaTime() const;

	Input* GetInput() const;

	void ConsoleLog(std::string str) const;
	const Shader& GetDefaultShader() const;

	void Quit();
};