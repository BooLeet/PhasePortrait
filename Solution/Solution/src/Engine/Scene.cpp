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

// Updates all objects or removes the ones with the destruction flag
void Scene::UpdateAllObjects()
{
	size_t destroyedObjects = 0;
	for (size_t i = 0; i < sceneObjects.size(); ++i)
	{
		if (sceneObjects[i]->GetDestructionFlag())
		{
			delete sceneObjects[i];
			sceneObjects[i] = sceneObjects[sceneObjects.size() - 1];
			sceneObjects.pop_back();
			--i;
			destroyedObjects++;
		}
		else
			sceneObjects[i]->UpdateBehaviours();
	}
}

size_t Scene::GetObjectCount() const
{
	return sceneObjects.size();
}
