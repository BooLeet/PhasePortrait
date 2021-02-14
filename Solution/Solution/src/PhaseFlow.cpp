#include "PhaseFlow.h"
#include "TrailRenderer.h"
#include "Engine/Engine.h"
#include "Engine/Scene.h"
#include "Engine/SceneObject.h"

#include "Engine/OpenGLComponents.h"
#include "Engine/ShaderLoader.h"
#include "Engine/vendor/imgui/imgui.h"
#include "Engine/vendor/imgui/imgui_impl_glfw_gl3.h"

#include "ExpressionCalculator.h"
#include "DifferentialEquation.h"


#include <string>

float PhaseFlow::CalculateDifferentialEquation(std::vector<float>& point)
{
	currentPointForCalculation = &point;
	
	return differentialEquation->rightSideExpression.EvaluateExpression(calculatorFunctions);
}

void PhaseFlow::CalculatorFunctionSetup()
{
	calculatorFunctions.insert(std::make_pair("+", [](std::stack<double>& args) {return StackPopReturn(args) + StackPopReturn(args); }));
	calculatorFunctions.insert(std::make_pair("-", [](std::stack<double>& args) {return -StackPopReturn(args) + StackPopReturn(args); }));
	calculatorFunctions.insert(std::make_pair("*", [](std::stack<double>& args) {return StackPopReturn(args) * StackPopReturn(args); }));
	calculatorFunctions.insert(std::make_pair("/", [](std::stack<double>& args) {return 1 / StackPopReturn(args) * StackPopReturn(args); }));
	calculatorFunctions.insert(std::make_pair("^", [](std::stack<double>& args) {double power = StackPopReturn(args);  return std::pow(StackPopReturn(args),power); }));

	calculatorFunctions.insert(std::make_pair("sin", [](std::stack<double>& args) {return std::sin(StackPopReturn(args)); }));
	calculatorFunctions.insert(std::make_pair("cos", [](std::stack<double>& args) {return std::cos(StackPopReturn(args)); }));
	calculatorFunctions.insert(std::make_pair("tan", [](std::stack<double>& args) {return std::tan(StackPopReturn(args)); }));

	calculatorFunctions.insert(std::make_pair("asin", [](std::stack<double>& args) {return std::asin(StackPopReturn(args)); }));
	calculatorFunctions.insert(std::make_pair("acos", [](std::stack<double>& args) {return std::acos(StackPopReturn(args)); }));
	calculatorFunctions.insert(std::make_pair("atan", [](std::stack<double>& args) {return std::atan(StackPopReturn(args)); }));

	calculatorFunctions.insert(std::make_pair("sqrt", [](std::stack<double>& args) {return std::sqrt(StackPopReturn(args)); }));

	calculatorFunctions.insert(std::make_pair("sign", [](std::stack<double>& args) {double val = StackPopReturn(args); return val / abs(val); }));

	calculatorFunctions.insert(std::make_pair("diff", [this](std::stack<double>& args) {return (*currentPointForCalculation)[StackPopReturn(args)]; }));

}

std::vector<float> PhaseFlow::GetPhaseSpeedVector(std::vector<float>& point)
{
	std::vector<float> result(differentialEquation->GetOrder());

	for (size_t i = 0; i < differentialEquation->GetOrder() - 1; ++i)
		result[i] = point[i + 1];

	
	result[differentialEquation->GetOrder() - 1] = CalculateDifferentialEquation(point);

	return result;
}

vec3 PhaseFlow::GetNewTrailPosition(const std::vector<float>& phasePosition)
{
	vec3 result = vec3(0);
	if (differentialEquation->GetOrder() > xDiffOrder)
		result.x = phasePosition[xDiffOrder];

	if (differentialEquation->GetOrder() > yDiffOrder)
		result.y = phasePosition[yDiffOrder];

	if (differentialEquation->GetOrder() > zDiffOrder)
		result.z = phasePosition[zDiffOrder];

	return result;
}

void PhaseFlow::PhasePointsSetup()
{
	engine->ConsoleLog("PhasePointsSetup");

	for (std::pair<TrailRenderer*, std::vector<float> > point : phasePoints)
		point.first->GetSceneObject()->Destroy();

	phasePoints.clear();

	size_t iSize = differentialEquation->GetOrder() >= 1 ? sampleSize : 0;
	size_t jSize = differentialEquation->GetOrder() >= 2 ? sampleSize : 1;
	size_t kSize = differentialEquation->GetOrder() >= 3 ? sampleSize : 1;


	for (size_t i = 0; i < iSize; ++i)
		for (size_t j = 0; j < jSize; ++j)
			for (size_t k = 0; k < kSize; ++k)
			{
				SceneObject* obj = engine->GetScene()->CreateObject("Trail");
				TrailRenderer* trailRenderer = obj->AddBehaviour<TrailRenderer>();
				trailRenderer->programID = programID;
				trailRenderer->renderMode = renderMode;
				trailRenderer->sampleLifeTime = 0.05;
				trailRenderer->maxSamples = 25;

				std::vector<float> phasePosition(differentialEquation->GetOrder(), 0);
				if (differentialEquation->GetOrder() > xDiffOrder)
					phasePosition[xDiffOrder] = 2 * simulationRadius * (i / (float)(sampleSize - 1) - 0.5); //(float)i - (size / 2.0);

				if (differentialEquation->GetOrder() > yDiffOrder)
					phasePosition[yDiffOrder] = 2 * simulationRadius * (j / (float)(sampleSize - 1) - 0.5); //(float)j - (size / 2.0);

				if (differentialEquation->GetOrder() > zDiffOrder)
					phasePosition[zDiffOrder] = 2 * simulationRadius * (k / (float)(sampleSize - 1) - 0.5); //(float)k - (size / 2.0);

				obj->transform.SetPosition(GetNewTrailPosition(phasePosition));
				phasePoints.push_back(std::pair<TrailRenderer*, std::vector<float> >(trailRenderer, phasePosition));
			}
	engine->ConsoleLog(std::to_string(engine->GetScene()->GetObjectCount()));
}

void PhaseFlow::Start()
{
	CalculatorFunctionSetup();
	programID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");
}

void PhaseFlow::Update()
{
	if (startSimulation)
	{
		startSimulation = false;
		PhasePointsSetup();
	}

	//if (engine->GetInput()->GetKey(GLFW_KEY_K))

	for (std::pair<TrailRenderer*, std::vector<float> >& phasePoint : phasePoints)
	{
		std::vector<float> phaseSpeed = GetPhaseSpeedVector(phasePoint.second);
		for (size_t i = 0; i < differentialEquation->GetOrder(); ++i)
			phasePoint.second[i] += simulationSpeed * (float)(engine->GetDeltaTime()) * phaseSpeed[i];

		phasePoint.first->GetSceneObject()->transform.SetPosition(GetNewTrailPosition(phasePoint.second));
	}

	ImGui::Begin("Phase Flow Controls");
	static float f = 1;
	std::string expression = "diff(" + std::to_string(differentialEquation->GetOrder()) + ") = " + differentialEquation->rightSideExpression.GetString().c_str();
	ImGui::Text(expression.c_str());                           // Display some text (you can use a format string too)
	if (ImGui::Button("Start"))
		startSimulation = true;
	ImGui::SliderFloat("Time Scale", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
	engine->timeScale = f;

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}

void PhaseFlow::OnDestroy()
{
	for (std::pair<TrailRenderer*, std::vector<float> > point : phasePoints)
		point.first->GetSceneObject()->Destroy();
	glDeleteProgram(programID);
}

void PhaseFlow::SetDifferentialEquation(DifferentialEquation& differentialEquationReference)
{
	differentialEquation = &differentialEquationReference;
}
