#include "SphericalCoordinateBehaviour.h"
#include "Engine.h"
#include "Input.h"
#include "SceneObject.h"



// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>

using namespace glm;

void SphericalCoordinateBehaviour::Update()
{
	if (engine->GetInput()->GetKey(GLFW_KEY_RIGHT))
		angleU += engine->GetUnscaledDeltaTime();
	if (engine->GetInput()->GetKey(GLFW_KEY_LEFT))
		angleU -= engine->GetUnscaledDeltaTime();

	if (engine->GetInput()->GetKey(GLFW_KEY_E))
		angleW -= engine->GetUnscaledDeltaTime();
	if (engine->GetInput()->GetKey(GLFW_KEY_Q))
		angleW += engine->GetUnscaledDeltaTime();

	if (engine->GetInput()->GetKey(GLFW_KEY_UP))
		angleV += engine->GetUnscaledDeltaTime();
	if (engine->GetInput()->GetKey(GLFW_KEY_DOWN))
		angleV -= engine->GetUnscaledDeltaTime();

	if (abs(angleV) >= pi<float>() / 2)
		angleV = sign(angleV) * pi<float>() / 2;

	double currentZoomSpeed = zoomSpeed;
	if (engine->GetInput()->GetKey(GLFW_KEY_LEFT_SHIFT))
		currentZoomSpeed *= 5;

	if (engine->GetInput()->GetKey(GLFW_KEY_W))
		distance -= currentZoomSpeed * engine->GetUnscaledDeltaTime();
	if (engine->GetInput()->GetKey(GLFW_KEY_S))
		distance += currentZoomSpeed * engine->GetUnscaledDeltaTime();

	vec3 rotation = vec3(-angleV, angleU, angleW);
	vec3 position = vec3(distance * sin(angleU) * cos(angleV),
		distance * sin(angleV),
		distance * cos(angleU) * cos(angleV));

	sceneObject->transform.SetPosition(position);
	sceneObject->transform.SetRotation(rotation);
}
