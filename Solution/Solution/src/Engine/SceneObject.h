#pragma once
#include "Transform.h"

#include <string>
#include <type_traits>
#include <algorithm>
#include <vector>

#include <iostream>

class Engine;

class Scene;

class ObjectBehaviour;

class SceneObject
{
private:
	Scene* scene;
	Engine* engine;
	std::vector<ObjectBehaviour*> behaviours;

	void RemoveBehaviour(const std::vector<ObjectBehaviour*>::iterator& iterator);

public:
	Transform transform;
	std::string name;

	SceneObject(Scene* scene, std::string name);
	~SceneObject();

	// Returns an engine pointer
	Engine* GetEnginePointer() const { return engine; }

	// Removes an object from the scene
	void Destroy();

	// Removes all behaviours from the object
	void RemoveBehaviours();

	// Returns a pointer to the behaviour or nullptr if not found
	template <typename T>
	T* GetBehaviour()
	{
		static_assert(std::is_base_of<ObjectBehaviour, T>::value, "type parameter of this function must derive from ObjectBehaviour");
		for (ObjectBehaviour* behaviour : behaviours)
		{
			if (std::is_same<typeid(*behaviour), T>::value)
				return behaviour;
		}

		return nullptr;
	}

	// Returns a pointer to the added behaviour or nullptr if the same behaviour is found
	template <typename T>
	T* AddBehaviour()
	{
		static_assert(std::is_base_of<ObjectBehaviour, T>::value, "type parameter of this function must derive from ObjectBehaviour");

		for (ObjectBehaviour* behaviour : behaviours)
		{
			if (typeid(*behaviour).name() == typeid(T).name())//(std::is_same<typeid(*behaviour), T>::value)
				return nullptr;
		}

		ObjectBehaviour* newBehaviour = new T();
		newBehaviour->ValidateBehaviour(this);
		behaviours.push_back(newBehaviour);

		return (T*)newBehaviour;
	}

	// Finds and removes a behaviour of type T
	template <typename T>
	void RemoveBehaviour()
	{
		static_assert(std::is_base_of<ObjectBehaviour, T>::value, "type parameter of this function must derive from ObjectBehaviour");

		std::vector<ObjectBehaviour*>::iterator iterator = std::find_if(behaviours.begin(), behaviours.end(), [](ObjectBehaviour* behaviour)
			{
				return std::is_same<typeid(*behaviour), T>::value;
			}
		);

		if (iterator == behaviours.end())
			return;

		RemoveBehaviour(iterator);

		/*(*iterator)->OnDestroy();
		std::iter_swap(iterator, behaviours.end() - 1);
		behaviours.pop_back();*/
	}

	// Calls Update and LateUpdate on all behaviours
	void UpdateBehaviours();
};