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

void SphericalCoordinateBehaviour::LateUpdate()
{
	vec3 rotation = vec3(-angleV, angleU, angleW);
	vec3 position = vec3(distance * sin(angleU) * cos(angleV),
		distance * sin(angleV),
		distance * cos(angleU) * cos(angleV));
	position = position + pivot;

	GetSceneObject()->GetTransform().SetPosition(position);
	GetSceneObject()->GetTransform().SetRotation(rotation);
}

void SphericalCoordinateBehaviour::Reset(float newDistance)
{
	angleU = angleV = angleW = 0;
	distance = newDistance;
}

void SphericalCoordinateBehaviour::HandleInput()
{
	if (GetEngine()->GetInput()->GetKey(GLFW_KEY_RIGHT))
		angleU += GetEngine()->GetUnscaledDeltaTime();
	if (GetEngine()->GetInput()->GetKey(GLFW_KEY_LEFT))
		angleU -= GetEngine()->GetUnscaledDeltaTime();

	if (GetEngine()->GetInput()->GetKey(GLFW_KEY_E))
		angleW -= GetEngine()->GetUnscaledDeltaTime();
	if (GetEngine()->GetInput()->GetKey(GLFW_KEY_Q))
		angleW += GetEngine()->GetUnscaledDeltaTime();

	if (GetEngine()->GetInput()->GetKey(GLFW_KEY_UP))
		angleV += GetEngine()->GetUnscaledDeltaTime();
	if (GetEngine()->GetInput()->GetKey(GLFW_KEY_DOWN))
		angleV -= GetEngine()->GetUnscaledDeltaTime();

	if (abs(angleV) >= pi<float>() / 2)
		angleV = sign(angleV) * pi<float>() / 2;

	double currentZoomSpeed = zoomSpeed;
	if (GetEngine()->GetInput()->GetKey(GLFW_KEY_LEFT_SHIFT))
		currentZoomSpeed *= 5;

	if (GetEngine()->GetInput()->GetKey(GLFW_KEY_W))
		distance -= currentZoomSpeed * GetEngine()->GetUnscaledDeltaTime();
	if (GetEngine()->GetInput()->GetKey(GLFW_KEY_S))
		distance += currentZoomSpeed * GetEngine()->GetUnscaledDeltaTime();
}
