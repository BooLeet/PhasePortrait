#include "ObjectBehaviour.h"
#include "SceneObject.h"

// Validates a behaviour by assigning an owner scene object to it
void ObjectBehaviour::ValidateBehaviour(SceneObject* sceneObject)
{
	this->sceneObject = sceneObject;
	this->engine = sceneObject->GetEnginePointer();
}

void ObjectBehaviour::TryAwake()
{
	if (removeFlag)
		return;

	if (enabled && !hasAwoken)
	{
		hasAwoken = true;
		Awake();
	}
}

void ObjectBehaviour::TryStart()
{
	if (removeFlag)
		return;

	if (enabled && !hadStarted)
	{
		hadStarted = true;
		Start();
	}
}

void ObjectBehaviour::TryUpdate()
{
	if (!enabled || removeFlag)
		return;
	Update();
}

void ObjectBehaviour::TryLateUpdate()
{
	if (!enabled || removeFlag)
		return;
	LateUpdate();
}

SceneObject* ObjectBehaviour::GetSceneObject() const
{
	return sceneObject;
}

Engine* ObjectBehaviour::GetEngine() const
{
	return engine;
}
