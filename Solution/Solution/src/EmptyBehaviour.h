#pragma once
#include "Engine/ObjectBehaviour.h"

#include <vector>
#include <deque>

class EmptyBehaviour : public RendererBehaviour
{
	std::deque<vec3>* deque;

	void Start()
	{ 
		deque = new std::deque<vec3>();
	}
	void Update(){}

	void Render(mat4 projectionViewMatrix){}

	void OnDestroyRenderer()
	{
		delete deque;
	}
};