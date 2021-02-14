#pragma once

#include "Engine/EngineLibrary.h"

class CoordinateSystemRenderer : public RendererBehaviour
{
	double radius = 10;
	double segmentLength = 1;
	double segmentNotchSize = 0.25;
	Mesh* mesh = nullptr;
	GLuint programID;

	bool xAxis = true;
	bool yAxis = true;
	bool zAxis = false;

	void OnDestroyRenderer();

	void CreateMesh();

public:
	void Render(mat4 projectionViewMatrix);

	void Start();

	void SetRadius(double newRadius);

	void SetSegmentLength(double newLength);

	void SetAxis(bool x, bool y, bool z);
};