#include "RendererBehaviour.h"
#include "Engine.h"

void RendererBehaviour::Awake()
{
	engine->rendererRegistry.RegisterRenderer(this);
}

void RendererBehaviour::OnDestroy()
{
	engine->rendererRegistry.UnregisterRenderer(this);
}