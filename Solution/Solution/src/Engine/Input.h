#pragma once

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

class Input
{
private:
	GLFWwindow* window;
public:
	Input(GLFWwindow* window):window(window){}
	bool GetKey(int key);
};

