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
	destructionFlag = false;
}

void SceneObject::Destroy()
{
	destructionFlag = true;
}

bool SceneObject::GetDestructionFlag() const
{
	return destructionFlag;
}


SceneObject::~SceneObject()
{
	RemoveBehaviours();
}

void SceneObject::RemoveBehaviour(const std::vector<ObjectBehaviour*>::iterator& iterator)
{
	(*iterator)->MarkToRemove();
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
	for (size_t i = 0; i < behaviours.size(); ++i)
	{
		if (behaviours[i]->GetRemoveFlag())
		{
			behaviours[i]->OnDestroy();
			delete behaviours[i];
			behaviours[i] = behaviours[behaviours.size() - 1];
			behaviours.pop_back();
		}
	}

	for (size_t i = 0; i < behaviours.size(); ++i)
		behaviours[i]->TryAwake();

	for (size_t i = 0; i < behaviours.size(); ++i)
		behaviours[i]->TryStart();

	for (size_t i = 0; i < behaviours.size(); ++i)
		behaviours[i]->TryUpdate();

	for (size_t i = 0; i < behaviours.size(); ++i)
		behaviours[i]->TryLateUpdate();
}

Transform& SceneObject::GetTransform()
{
	return transform;
}
