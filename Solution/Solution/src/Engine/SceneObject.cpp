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
	shouldBeDestroyed = false;
}

void SceneObject::Destroy()
{
	shouldBeDestroyed = true;
	//scene->DestroyObject(this);
}

bool SceneObject::GetDestructionFlag() const
{
	return shouldBeDestroyed;
}


SceneObject::~SceneObject()
{
	RemoveBehaviours();
}

void SceneObject::RemoveBehaviour(const std::vector<ObjectBehaviour*>::iterator& iterator)
{
	(*iterator)->MarkToRemove();

	/*(*iterator)->OnDestroy();
	delete* iterator;
	std::iter_swap(iterator, behaviours.end() - 1);
	behaviours.pop_back();*/
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
		if (behaviours[i]->GetRemoveMark())
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
		behaviours[i]->TryUpdate();

	for (size_t i = 0; i < behaviours.size(); ++i)
		behaviours[i]->TryLateUpdate();

	/*for (ObjectBehaviour* behaviour : behaviours)
		behaviour->TryAwake();

	for (ObjectBehaviour* behaviour : behaviours)
		behaviour->TryUpdate();

	for (ObjectBehaviour* behaviour : behaviours)
		behaviour->TryLateUpdate();*/
}
