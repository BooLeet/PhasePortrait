// Include standard headers
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
using namespace glm;

//#include "Engine/ShaderLoader.h"

// Time Counter
#include <chrono> 
using namespace std::chrono;

#include "Engine/Engine.h"
#include "Engine/Scene.h"
#include "Engine/SceneObject.h"
#include "Engine/ColoredCubeRenderer.h"
#include "Engine/CameraBehaviour.h"
#include "Engine/SphericalCoordinateBehaviour.h"
#include "TrailRenderer.h"

int main(void)
{
	Engine engine = Engine(1280, 720);
	SceneObject* camera = engine.GetScene()->CreateObject("Camera");
	camera->AddBehaviour<CameraBehaviour>();
	camera->AddBehaviour<SphericalCoordinateBehaviour>();
	camera->transform.SetPosition(vec3(0, 0, 0));

	SceneObject* coloredCube = engine.GetScene()->CreateObject("ColoredCube");
	coloredCube->AddBehaviour<ColoredCubeRenderer>();
	coloredCube->transform.SetRotation(vec3(0, 0, 0));
	coloredCube->transform.SetPosition(vec3(0,0,0));
	coloredCube->transform.SetScale(vec3(50, 50, 50));

	/*SceneObject* coloredCube1 = engine.GetScene()->CreateObject("ColoredCube1");
	coloredCube1->AddBehaviour<ColoredCubeRenderer>();
	coloredCube1->transform.SetRotation(vec3(0, 0, 0));
	coloredCube1->transform.SetPosition(vec3(0, 0, 0)); */

	SceneObject* trailTest = engine.GetScene()->CreateObject("Trail");
	TrailRenderer* trailRenderer = trailTest->AddBehaviour<TrailRenderer>();
	trailRenderer->renderMode = TrailRenderer::RenderMode::Triangles;
	trailTest->transform.SetRotation(vec3(0, 0, 0));
	trailTest->transform.SetPosition(vec3(-18, 0, 0));
	

	engine.MainLoop();
}

