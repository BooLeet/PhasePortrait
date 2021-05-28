#include "CameraBehaviour.h"
#include "Engine.h"
#include "SceneObject.h"
#include "Transform.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

void CameraBehaviour::Awake()
{
	GetEngine()->cameraRegistry.RegisterCamera(this);
}

void CameraBehaviour::OnDestroy()
{
	GetEngine()->cameraRegistry.UnregisterCamera(this);
}

mat4 CameraBehaviour::GetProjectionViewMatrix() const
{
	size_t width = GetEngine()->GetWindowWidth();
	size_t height = GetEngine()->GetWindowHeight();
	glm::mat4 Projection = glm::perspective(glm::radians(fieldOfView), width / (float)height, nearPlane, farPlane);

	glm::mat4 rotationMatrix = GetSceneObject()->GetTransform().GetRotationMatrix();
	glm::mat4 View = glm::inverse(rotationMatrix);
	glm::mat4 World = glm::translate(glm::mat4(1.0), -(GetSceneObject()->GetTransform().GetPosition()));
	return Projection * View * World;
}
