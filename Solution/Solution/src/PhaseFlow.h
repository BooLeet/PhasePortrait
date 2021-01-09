#pragma once
#include "Engine/ObjectBehaviour.h"
#include "TrailRenderer.h"
#include <vector>

//class TrailRenderer;

class PhaseFlow : public ObjectBehaviour
{
	std::vector<TrailRenderer*> trails;
	size_t size = 15;
public:
	float airResistance = 0.5;
	float g = 10;
	float pendulumLength = 1;
	TrailRenderer::RenderMode renderMode = TrailRenderer::RenderMode::Lines;

	float simulationSpeed = 1;

	void Start();
	void Update();

	void OnDestroy();
};