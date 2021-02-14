#pragma once
#include "ObjectBehaviour.h"
// Include GLM
#include <glm/glm.hpp>
using namespace glm;

class RendererBehaviour : public ObjectBehaviour
{
protected:
	virtual void OnDestroyRenderer() {}

public:
	void TryRender(mat4 projectionViewMatrix);

	virtual void Render(mat4 projectionViewMatrix) = 0;

	void Awake();

	void OnDestroy();
};