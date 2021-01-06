#include "Input.h"

bool Input::GetKey(int key)
{
	int state = glfwGetKey(window, key);
	return state == GLFW_PRESS;
}
