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
public:
	std::string name;
private:
	Transform transform;
	bool destructionFlag;

	Scene* scene;
	Engine* engine;
	std::vector<ObjectBehaviour*> behaviours;

public:
	SceneObject(Scene* scene, std::string name);
	~SceneObject();

	// Returns the object's transform
	Transform& GetTransform();

	// Returns an engine pointer
	Engine* GetEnginePointer() const { return engine; }

	// Sets the destruction flag to true
	void Destroy();

	bool GetDestructionFlag() const;

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
	}

	// Calls Update and LateUpdate on all behaviours
	void UpdateBehaviours();

private:
	// Removes a given behaviour
	void RemoveBehaviour(const std::vector<ObjectBehaviour*>::iterator& iterator);
};