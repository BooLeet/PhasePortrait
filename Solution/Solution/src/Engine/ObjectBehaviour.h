#pragma once
class SceneObject;

class Engine;

class ObjectBehaviour
{
private:
	bool hasAwoken = false;
protected:
	SceneObject* sceneObject;
	Engine* engine;
public:
	bool enabled = true;
	// Validates a behaviour by assigning an owner scene object to it
	void ValidateBehaviour(SceneObject* sceneObject);

	void TryAwake();

	void TryUpdate();

	void TryLateUpdate();

	void virtual Awake() {};
	void virtual Start() {};
	void virtual Update() {};
	void virtual LateUpdate() {};
	void virtual OnDestroy() {};

	SceneObject* GetSceneObject();
};