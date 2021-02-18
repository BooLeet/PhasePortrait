#pragma once

#pragma once
#include "Engine/ObjectBehaviour.h"

#include <vector>

class MemLeakTester : public ObjectBehaviour
{
	std::vector<SceneObject*> objects;

public:
	
	void Start();
	void Update();

	void OnDestroy();

private:
	void CreateObjects();
	void ClearObjects();
};
