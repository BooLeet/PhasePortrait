#pragma once
#include "ObjectBehaviour.h"

class SphericalCoordinateBehaviour : public ObjectBehaviour
{
public:
	float angleU = 0;
	float angleV = 0;
	float angleW = 0;
	float distance = 5;

	float zoomSpeed = 2;

	void Update();
};