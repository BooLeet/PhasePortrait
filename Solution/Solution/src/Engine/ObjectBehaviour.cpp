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
	if (shouldBeRemoved)
		return;

	if (enabled && !hasAwoken)
	{
		hasAwoken = true;
		Awake();
		Start();
	}
}

void ObjectBehaviour::TryUpdate()
{
	if (!enabled || shouldBeRemoved)
		return;
	Update();
}

void ObjectBehaviour::TryLateUpdate()
{
	if (!enabled || shouldBeRemoved)
		return;
	LateUpdate();
}

SceneObject* ObjectBehaviour::GetSceneObject()
{
	return sceneObject;
}
