#pragma once
#include "Engine/EngineLibrary.h"
#include <deque>
class TrailRenderer : public RendererBehaviour
{
private:
	std::deque<vec3>* samples;

	float sampleTimeCounter = 0;
public:
	enum class RenderMode { Lines, Triangles, Points };
	RenderMode renderMode = RenderMode::Lines;
	float sampleLifeTime = 0.1;
	size_t maxSamples = 10;
	vec3 trailHeadColor = vec3(1, 0, 0);
	vec3 trailTailColor = vec3(0, 0, 1);

	void Render(mat4 projectionViewMatrix);

	void Start();

	void Update();

	void OnDestroyRenderer();

	void ClearTrail();
};
