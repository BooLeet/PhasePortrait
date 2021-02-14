#include "Scene.h"
#include "Engine.h"
#include "SceneObject.h"
#include <algorithm>

// Adds an object to the scene
SceneObject* Scene::CreateObject(std::string name)
{
	SceneObject* obj = new SceneObject(this, name);
	sceneObjects.push_back(obj);
	return obj;
}

// Finds and removes an object from the scene
void Scene::DestroyObject(SceneObject* obj)
{
	auto iterator = std::find(sceneObjects.begin(), sceneObjects.end(), obj);
	if (iterator == sceneObjects.end())
		return;

	delete *iterator;
	
	std::iter_swap(iterator, sceneObjects.end() - 1);
	sceneObjects.pop_back();
}

void Scene::UpdateAllObjects()
{
	for (SceneObject* obj : sceneObjects)
		obj->UpdateBehaviours();
}

size_t Scene::GetObjectCount() const
{
	return sceneObjects.size();
}
