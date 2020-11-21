#include "Transform.h"
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>


Transform::Transform(const vec3& position, const vec3& rotation, const vec3& scale)
{
	this->position = position;
	this->rotation = rotation;
	this->scale = scale;
	UpdateDirectionVectors();
}

void Transform::SetPosition(const vec3& newPosition)
{
	this->position = newPosition;
}

void Transform::SetRotation(const vec3& newRotation)
{
	this->rotation = newRotation;
	UpdateDirectionVectors();
}

void Transform::SetScale(const vec3& newScale)
{
	this->scale = newScale;
}

vec3 Transform::GetPosition() const
{
	return position;
}

vec3 Transform::GetRotation() const
{
	return rotation;
}

vec3 Transform::GetScale() const
{
	return scale;
}


void Transform::UpdateDirectionVectors()
{
	

	/*forward = glm::vec3(0, 0, 1);

	forward = glm::rotate(forward, rotation.z, glm::vec3(0, 0, 1));
	forward = glm::rotate(forward, rotation.x, glm::vec3(1, 0, 0));
	forward = glm::rotate(forward, rotation.y, glm::vec3(0, 1, 0));

	up = glm::vec3(0, 1, 0);

	up = glm::rotate(up, rotation.z, glm::vec3(0, 0, 1));
	up = glm::rotate(up, rotation.x, glm::vec3(1, 0, 0));
	up = glm::rotate(up, rotation.y, glm::vec3(0, 1, 0));

	right = glm::vec3(1, 0, 0);

	right = glm::rotate(right, rotation.z, glm::vec3(0, 0, 1));
	right = glm::rotate(right, rotation.x, glm::vec3(1, 0, 0));
	right = glm::rotate(right, rotation.y, glm::vec3(0, 1, 0));*/

	forward = glm::vec3(0, 0, 1);

	forward = glm::rotate(forward, rotation.y, glm::vec3(0, 1, 0));
	forward = glm::rotate(forward, rotation.x, glm::vec3(1, 0, 0));
	forward = glm::rotate(forward, rotation.z, glm::vec3(0, 0, 1));
	
	

	up = glm::vec3(0, 1, 0);

	up = glm::rotate(up, rotation.y, glm::vec3(0, 1, 0));
	up = glm::rotate(up, rotation.x, glm::vec3(1, 0, 0));
	up = glm::rotate(up, rotation.z, glm::vec3(0, 0, 1));
	
	

	right = glm::vec3(1, 0, 0);

	right = glm::rotate(right, rotation.y, glm::vec3(0, 1, 0));
	right = glm::rotate(right, rotation.x, glm::vec3(1, 0, 0));
	right = glm::rotate(right, rotation.z, glm::vec3(0, 0, 1));
	
	
}

vec3 Transform::GetForward() const
{
	return forward;
}

vec3 Transform::GetUp() const
{
	return up;
}

vec3 Transform::GetRight() const
{
	return right;
}

mat4 Transform::GetModelMatrix() const
{
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0), position);
	glm::mat4 rotationMatrix = glm::toMat4(glm::quat(rotation));
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0), scale);
	return translationMatrix * rotationMatrix * scaleMatrix; //glm::lookAt(GetPosition(), GetPosition() + GetForward(), GetUp());
}
