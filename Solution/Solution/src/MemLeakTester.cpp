#include "MemLeakTester.h"
#include "Engine/EngineLibrary.h"
#include "EmptyBehaviour.h"
#include "TrailRenderer.h"
#include "Engine/SphericalCoordinateBehaviour.h"

void MemLeakTester::Start()
{
}

void MemLeakTester::Update()
{
	if (engine->GetInput()->GetKey(GLFW_KEY_K))
		CreateObjects();
	if (engine->GetInput()->GetKey(GLFW_KEY_L))
		engine->ConsoleLog(std::to_string(engine->GetScene()->GetObjectCount()) + " " + std::to_string(objects.size()));
	if (engine->GetInput()->GetKey(GLFW_KEY_J))
		ClearObjects();
}

void MemLeakTester::OnDestroy()
{
}

void MemLeakTester::CreateObjects()
{
	engine->ConsoleLog("\nCreateObjects Start");
	engine->ConsoleLog(std::to_string(engine->GetScene()->GetObjectCount()) + " " + std::to_string(objects.size()));
	ClearObjects();
	for (size_t i = 0; i < 20000; ++i)
	{
		SceneObject* newObject = engine->GetScene()->CreateObject(std::to_string(i) + " object");
		newObject->AddBehaviour<EmptyBehaviour>();
		objects.push_back(newObject);
	}
	engine->ConsoleLog("CreateObjects Complete");
	
}

void MemLeakTester::ClearObjects()
{
	engine->ConsoleLog("ClearObjects Start");
	for (SceneObject* obj : objects)
		obj->Destroy();
	objects.clear();
	engine->ConsoleLog("ClearObjects Complete");
}
