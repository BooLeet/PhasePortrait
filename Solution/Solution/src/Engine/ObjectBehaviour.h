#pragma once
class SceneObject;

class Engine;

class ObjectBehaviour
{
public:
	bool enabled = true;
private:
	bool hasAwoken = false;
	bool hadStarted = false;
	bool removeFlag = false;
protected:
	SceneObject* sceneObject;
	Engine* engine;

public:
	// Validates a behaviour by assigning an owner scene object to it
	void ValidateBehaviour(SceneObject* sceneObject);

	void TryAwake();

	void TryStart();

	void TryUpdate();

	void TryLateUpdate();

	void virtual Awake() {};
	void virtual Start() {};
	void virtual Update() {};
	void virtual LateUpdate() {};
	void virtual OnDestroy() {};

	SceneObject* GetSceneObject();

	void MarkToRemove() { removeFlag = true; }
	bool GetRemoveFlag() const { return removeFlag; }
};