#include "PhaseFlow.h"
#include "TrailRenderer.h"
#include "Engine/Engine.h"
#include "Engine/Scene.h"
#include "Engine/SceneObject.h"

#include "Engine/OpenGLComponents.h"
#include "Engine/ShaderLoader.h"

#include "ExpressionCalculator.h"

float PhaseFlow::CalculateDifferentialEquation(const std::vector<float>& point)
{
	return EvaluateExpression(rightSideExpression, calculatorFunctions);
	//return -airResistance * point[1] - (g / pendulumLength) * sin(point[0]);
}

void PhaseFlow::CalculatorFunctionSetup()
{
	function plus = [](std::stack<double>& args) {return StackPopReturn(args) + StackPopReturn(args); };
	function minus = [](std::stack<double>& args) {return -StackPopReturn(args) + StackPopReturn(args); };
	function multiply = [](std::stack<double>& args) {return StackPopReturn(args) * StackPopReturn(args); };
	function divide = [](std::stack<double>& args) {return 1 / StackPopReturn(args) * StackPopReturn(args); };
	function sin = [](std::stack<double>& args) {return std::sin(StackPopReturn(args)); };
	function cos = [](std::stack<double>& args) {return std::cos(StackPopReturn(args)); };

	function diff = [this](std::stack<double>& args) {return (*currentPointForCalculation)[StackPopReturn(args)]; };

	calculatorFunctions.insert(std::make_pair("+", plus));
	calculatorFunctions.insert(std::make_pair("-", minus));
	calculatorFunctions.insert(std::make_pair("*", multiply));
	calculatorFunctions.insert(std::make_pair("/", divide));
	calculatorFunctions.insert(std::make_pair("sin", sin));
	calculatorFunctions.insert(std::make_pair("cos", cos));
	calculatorFunctions.insert(std::make_pair("diff", diff));

}

std::vector<float> PhaseFlow::GetPhaseSpeedVector(std::vector<float>& point)
{
	std::vector<float> result(differentialEquationOrder);

	for (size_t i = 0; i < differentialEquationOrder - 1; ++i)
		result[i] = point[i + 1];

	currentPointForCalculation = &point;
	result[differentialEquationOrder - 1] = CalculateDifferentialEquation(point);

	return result;
}

vec3 PhaseFlow::GetNewTrailPosition(const std::vector<float>& phasePosition)
{
	vec3 result = vec3(0);
	if (differentialEquationOrder > xDiffOrder)
		result.x = phasePosition[xDiffOrder];

	if (differentialEquationOrder > yDiffOrder)
		result.y = phasePosition[yDiffOrder];

	if (differentialEquationOrder > zDiffOrder)
		result.z = phasePosition[zDiffOrder];

	return result;
}

void PhaseFlow::Start()
{
	CalculatorFunctionSetup();

	GLuint programID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");
	size_t iSize = differentialEquationOrder >= 1 ? sampleSize : 0;
	size_t jSize = differentialEquationOrder >= 2 ? sampleSize : 1;
	size_t kSize = differentialEquationOrder >= 3 ? sampleSize : 1;

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
			
				std::vector<float> phasePosition(differentialEquationOrder, 0);
				if (differentialEquationOrder > xDiffOrder)
					phasePosition[xDiffOrder] = 2 * simulationRadius * (i / (float)(sampleSize - 1) - 0.5); //(float)i - (size / 2.0);

				if (differentialEquationOrder > yDiffOrder)
					phasePosition[yDiffOrder] = 2 * simulationRadius * (j / (float)(sampleSize - 1) - 0.5); //(float)j - (size / 2.0);

				if (differentialEquationOrder > zDiffOrder)
					phasePosition[zDiffOrder] = 2 * simulationRadius * (k / (float)(sampleSize - 1) - 0.5); //(float)k - (size / 2.0);

				obj->transform.SetPosition(GetNewTrailPosition(phasePosition));
				phasePoints.push_back(std::pair<TrailRenderer*, std::vector<float> >(trailRenderer, phasePosition));
			}
}

void PhaseFlow::Update()
{
	for (std::pair<TrailRenderer*, std::vector<float> >& phasePoint : phasePoints)
	{
		std::vector<float> phaseSpeed = GetPhaseSpeedVector(phasePoint.second);
		for (size_t i = 0; i < differentialEquationOrder; ++i)
			phasePoint.second[i] += simulationSpeed * (float)(engine->GetDeltaTime()) * phaseSpeed[i];

		phasePoint.first->GetSceneObject()->transform.SetPosition(GetNewTrailPosition(phasePoint.second));
	}
}

void PhaseFlow::OnDestroy()
{
	for (size_t i = 0; i < phasePoints.size(); ++i)
		delete phasePoints[i].first;
	//glDeleteProgram(programID);
}
