#pragma once

#include "Engine/EngineLibrary.h"

class CoordinateSystemRenderer : public RendererBehaviour
{
	double radius = 10;
	double segmentLength = 1;
	double segmentNotchSize = 0.25;
	Mesh* mesh = nullptr;

	bool xAxis = true;
	bool yAxis = true;
	bool zAxis = false;

	void OnDestroyRenderer();

	void CreateMesh();
	vec3 xColor = vec3(0.7, 0.7, 0.7);
	vec3 yColor = vec3(0.7, 0.7, 0.7);
	vec3 zColor = vec3(0.7, 0.7, 0.7);

public:
	void Render(mat4 projectionViewMatrix);

	void Start();

	void SetRadius(double newRadius);

	void SetSegmentLength(double newLength);

	void SetSegmentNotchSize(double newSize);

	void SetAxis(bool x, bool y, bool z);

	void SetColorX(vec3 color);

	void SetColorY(vec3 color);

	void SetColorZ(vec3 color);

	vec3 GetColorX() const { return xColor; };

	vec3 GetColorY() const { return yColor; };

	vec3 GetColorZ() const { return zColor; };
};