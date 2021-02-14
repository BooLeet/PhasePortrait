#include "SceneObject.h"
#include "Scene.h"
#include "Engine.h"
#include "ObjectBehaviour.h"

SceneObject::SceneObject(Scene* scene, std::string name)
{
	this->scene = scene;
	this->engine = scene->GetEnginePointer();
	this->transform = Transform();
	this->name = name;
}

void SceneObject::Destroy()
{
	scene->DestroyObject(this);
}

SceneObject::~SceneObject()
{
	RemoveBehaviours();
}

void SceneObject::RemoveBehaviour(const std::vector<ObjectBehaviour*>::iterator& iterator)
{
	(*iterator)->OnDestroy();
	delete* iterator;
	std::iter_swap(iterator, behaviours.end() - 1);
	behaviours.pop_back();
}

void SceneObject::RemoveBehaviours()
{
	for (ObjectBehaviour* behaviour : behaviours)
	{
		behaviour->OnDestroy();
		delete behaviour;
	}

	behaviours.clear();
}

void SceneObject::UpdateBehaviours()
{
	for (ObjectBehaviour* behaviour : behaviours)
		behaviour->TryAwake();

	for (ObjectBehaviour* behaviour : behaviours)
		behaviour->TryUpdate();

	for (ObjectBehaviour* behaviour : behaviours)
		behaviour->TryLateUpdate();
}
