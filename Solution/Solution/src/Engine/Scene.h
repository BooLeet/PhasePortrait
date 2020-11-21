#pragma once
#include <vector>
#include <string>

class Engine;

class SceneObject;

class Scene
{
private:
	Engine* engine;
	std::vector<SceneObject*> sceneObjects;
public:
	Scene(Engine* engine) :engine(engine) {}
	// Adds an object to the scene
	SceneObject* CreateObject(std::string name);
	// Finds and removes an object from the scene
	void DestroyObject(SceneObject* obj);

	// Returns an engine pointer
	Engine* GetEnginePointer() const { return engine; }

	void UpdateAllObjects();
};