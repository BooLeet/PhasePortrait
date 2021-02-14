#include "RendererBehaviour.h"
#include "Engine.h"

void RendererBehaviour::Awake()
{
	engine->rendererRegistry.RegisterRenderer(this);
}

void RendererBehaviour::OnDestroy()
{
	OnDestroyRenderer();
	engine->rendererRegistry.UnregisterRenderer(this);
}