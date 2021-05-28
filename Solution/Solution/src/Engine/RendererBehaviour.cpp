#include "RendererBehaviour.h"
#include "Engine.h"

void RendererBehaviour::TryRender(mat4 projectionViewMatrix)
{
	if (enabled)
		Render(projectionViewMatrix);
}

void RendererBehaviour::Awake()
{
	GetEngine()->rendererRegistry.RegisterRenderer(this);
}

void RendererBehaviour::OnDestroy()
{
	OnDestroyRenderer();
	GetEngine()->rendererRegistry.UnregisterRenderer(this);
}