#pragma once
#include "ObjectBehaviour.h"
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
using namespace glm;

class CameraBehaviour : public ObjectBehaviour
{
public:
	float fieldOfView = 75;
	float nearPlane = 0.01f;
	float farPlane = 500;

	void Awake();

	void OnDestroy();

	mat4 GetProjectionViewMatrix() const;
};