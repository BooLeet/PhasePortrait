#include "CameraBehaviour.h"
#include "Engine.h"
#include "SceneObject.h"
#include "Transform.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

void CameraBehaviour::Awake()
{
	engine->cameraRegistry.RegisterCamera(this);
}

void CameraBehaviour::OnDestroy()
{
	engine->cameraRegistry.UnregisterCamera(this);
}

mat4 CameraBehaviour::GetProjectionViewMatrix() const
{
	size_t width = engine->GetWindowWidth();
	size_t height = engine->GetWindowHeight();
	glm::mat4 Projection = glm::perspective(glm::radians(fieldOfView), width / (float)height, nearPlane, farPlane);

	glm::mat4 rotationMatrix = glm::toMat4(glm::quat(sceneObject->transform.GetRotation()));
	/*vec3 camPos = sceneObject->transform.GetPosition();
	vec3 camForward = sceneObject->transform.GetForward();
	vec3 camUp = sceneObject->transform.GetUp();
	glm::mat4 cameraRot = (glm::lookAt(-camPos, -camPos + camForward, camUp));*/
	glm::mat4 View = glm::inverse(rotationMatrix);
	glm::mat4 World = glm::translate(glm::mat4(1.0), -(sceneObject->transform.GetPosition()));
	return Projection * View * World;
}
