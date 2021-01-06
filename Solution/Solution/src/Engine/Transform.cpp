#include "Transform.h"
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace glm;

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

void Transform::RotateAround(float angle, vec3 normal)
{
	rotation = glm::rotate(rotation, angle, normal);
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




	/*forward = glm::vec3(0, 0, 1);

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
	right = glm::rotate(right, rotation.z, glm::vec3(0, 0, 1));*/

	vec4 forward4 = vec4(0, 0, 1, 0);
	vec4 up4 = vec4(0, 1, 0, 0);
	vec4 right4 = vec4(1, 0, 0, 0);
	glm::mat4 rotationMatrix = GetRotationMatrix();
	forward4 = rotationMatrix * forward4;
	up4 = rotationMatrix * up4;
	right4 = rotationMatrix * right4;
	
	forward = vec3(forward4.x, forward4.y, forward4.z);
	up = vec3(up4.x, up4.y, up4.z);
	right = vec3(right4.x, right4.y, right4.z);
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

mat4 Transform::GetRotationMatrix() const
{
	//return glm::toMat4(glm::quat(rotation));

	mat4 result = mat4(1);
	
	
	result = rotate(result, rotation.y, vec3(0.0, 1.0, 0.0));
	result = rotate(result, rotation.x, vec3(1.0, 0.0, 0.0));
	result = rotate(result, rotation.z, vec3(0.0, 0.0, 1.0));
	
	return result;
}

mat4 Transform::GetModelMatrix() const
{
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0), position);
	glm::mat4 rotationMatrix = GetRotationMatrix();
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0), scale);
	return translationMatrix * rotationMatrix * scaleMatrix; //glm::lookAt(GetPosition(), GetPosition() + GetForward(), GetUp());
}
