#pragma once
#include "ObjectBehaviour.h"
#include "OpenGLComponents.h"

class SphericalCoordinateBehaviour : public ObjectBehaviour
{
public:
	vec3 pivot;
	float angleU = 0;
	float angleV = 0;
	float angleW = 0;
	float distance = 5;

	float zoomSpeed = 2;

	void LateUpdate();

	void Reset(float newDistance);

	void HandleInput();
};