#pragma once
#include <vector>
#include <string>

class Engine;

class SceneObject;

class Scene
{
private:
	// Для получения необходимого функционала Движка
	Engine* engine;
	// Контейнер всех объектов
	std::vector<SceneObject*> sceneObjects;
public:
	Scene(Engine* engine) :engine(engine) {}

	// Adds an object to the scene
	SceneObject* CreateObject(std::string name);

	// Returns an engine pointer
	Engine* GetEnginePointer() const { return engine; }

	// Updates all objects or removes the ones with the destruction flag
	void UpdateAllObjects();

	size_t GetObjectCount() const;
};