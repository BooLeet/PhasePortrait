#include "PhaseFlow.h"
#include "TrailRenderer.h"
#include "Engine/Engine.h"
#include "Engine/Scene.h"
#include "Engine/SceneObject.h"

#include "Engine/OpenGLComponents.h"

void PhaseFlow::Start()
{
	for (size_t i = 0; i < size; ++i)
		for (size_t j = 0; j < size; ++j)
		{
			SceneObject* obj = engine->GetScene()->CreateObject("Trail");
			TrailRenderer* trailRenderer = obj->AddBehaviour<TrailRenderer>();
			trailRenderer->renderMode = renderMode;
			trailRenderer->sampleLifeTime = 0.05;
			obj->transform.SetPosition(vec3((float)i - (size/2.0), (float)j - (size / 2.0), 0));
			trails.push_back(trailRenderer);
		}
}

void PhaseFlow::Update()
{
	for (TrailRenderer* trail : trails)
	{
		vec3 currentPosition = trail->GetSceneObject()->transform.GetPosition();
		vec3 deltaPosition = simulationSpeed * (float)(engine->GetDeltaTime()) * vec3(currentPosition.y, -airResistance * currentPosition.y - (g / pendulumLength) * sin(currentPosition.x), 0);
		trail->GetSceneObject()->transform.SetPosition(currentPosition + deltaPosition);
	}
}

void PhaseFlow::OnDestroy()
{
	for (TrailRenderer* trail : trails)
		delete trail;
}
