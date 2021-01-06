#pragma once
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

class Transform
{
private:
	vec3 forward, up, right;
	vec3 position;
	vec3 rotation;
	vec3 scale;

	void UpdateDirectionVectors();
public:
	Transform(const vec3& position = vec3(0), const vec3& rotation = vec3(0), const vec3& scale = vec3(1));

	void SetPosition(const vec3& newPosition);
	void SetRotation(const vec3& newRotation);
	void SetScale(const vec3& newScale);

	void RotateAround(float angle, vec3 normal);

	vec3 GetPosition() const;
	vec3 GetRotation() const;
	vec3 GetScale() const;

	vec3 GetForward() const;
	vec3 GetUp() const;
	vec3 GetRight() const;

	mat4 GetRotationMatrix() const; 
	mat4 GetModelMatrix() const;
};