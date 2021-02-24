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
#include "CoordinateSystemRenderer.h"

#include "Vector.h"


#include <string>
#include <fstream>
#include <strstream>


float PhaseFlow::CalculateDifferentialEquation(double t,const Vector<float>& point)
{
	currentPointForCalculation = point;
	differentialEquation->rightSideExpression.SetVariable("t", t);
	std::pair<double,std::string> result = differentialEquation->rightSideExpression.EvaluateExpressionAndGetError(calculatorFunctions);
	equationErrorMessage = result.second;
	if (result.second == "")
		return result.first;

	stopSimulation = true;
	return 0;
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

	calculatorFunctions.insert(std::make_pair("diff", [this](std::stack<double>& args) {return currentPointForCalculation[StackPopReturn(args)]; }));

}

void PhaseFlow::PresetEquationsSetup()
{
	presetEquations.push_back(DifferentialEquationInput(2, "(0-mu) * diff(1) - (g / L) * sin(diff(0))", "mu = 0.5\ng = 10\nL = 5", "Pendulum Equation"));
	presetEquations.push_back(DifferentialEquationInput(2, "mu*(1-diff(0)*diff(0))*diff(1)-diff(0)", "mu = 0.1", "Van der Pol oscillator"));
}

std::vector< std::pair<double, Vector<float>>> PhaseFlow::GetPhaseTrajectory(const std::vector<float>& startPosition, double startingTime, double endTime, double timeStep)
{
	if (startingTime > endTime)
		std::swap(startingTime, endTime);
	size_t steps = std::ceil((endTime - startingTime) / timeStep);

	double tPrev = startingTime;
	Vector<float> yPrev = startPosition;
	std::vector< std::pair<double, Vector<float>>> result;
	result.push_back(std::make_pair(tPrev, yPrev));

	for (size_t i = 1; i <= steps; ++i)
	{
		Vector<float> K1 = timeStep * GetPhaseSpeedVector(tPrev, yPrev);
		Vector<float> K2 = timeStep * GetPhaseSpeedVector(tPrev + timeStep/2, yPrev + K1);
		Vector<float> K3 = timeStep * GetPhaseSpeedVector(tPrev + timeStep / 2, yPrev + K2);
		Vector<float> K4 = timeStep * GetPhaseSpeedVector(tPrev + timeStep, yPrev + K3);

		Vector<float> deltaY = (K1 + 2 * K2 + 2 * K3 + K4) * (1/6.0);
		tPrev += timeStep;
		yPrev = yPrev + deltaY;
		result.push_back(std::make_pair(tPrev, yPrev));
	}

	return result;
}

Vector<float> PhaseFlow::GetPhaseSpeedVector(double t,const Vector<float>& point)
{
	std::vector<float> result(differentialEquation->GetOrder());

	for (size_t i = 0; i < differentialEquation->GetOrder() - 1; ++i)
		result[i] = point[i + 1];

	result[differentialEquation->GetOrder() - 1] = CalculateDifferentialEquation(t, point);

	return result;
}

vec3 PhaseFlow::GetNewTrailPosition(const Vector<float>& phasePosition)
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
	ClearPhasePoints();

	size_t samplePerDimension = sampleSize;
	if (differentialEquation->GetOrder() == 2)
		samplePerDimension = sqrt(sampleSize);
	else if (differentialEquation->GetOrder() >= 3)
		samplePerDimension = cbrt(sampleSize);

	size_t iSize = differentialEquation->GetOrder() >= 1 ? samplePerDimension : 0;
	size_t jSize = differentialEquation->GetOrder() >= 2 ? samplePerDimension : 1;
	size_t kSize = differentialEquation->GetOrder() >= 3 ? samplePerDimension : 1;


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
					phasePosition[xDiffOrder] = 2 * simulationRadius * (i / (float)(samplePerDimension - 1) - 0.5);

				if (differentialEquation->GetOrder() > yDiffOrder)
					phasePosition[yDiffOrder] = 2 * simulationRadius * (j / (float)(samplePerDimension - 1) - 0.5);

				if (differentialEquation->GetOrder() > zDiffOrder)
					phasePosition[zDiffOrder] = 2 * simulationRadius * (k / (float)(samplePerDimension - 1) - 0.5);

				obj->transform.SetPosition(GetNewTrailPosition(phasePosition));
				std::vector< std::pair<double, Vector<float>>> phaseTrajectory = GetPhaseTrajectory(phasePosition, simulationStartTimeVolatile, simulationEndTimeVolatile, simulationTimeStepVolatile);
				phasePoints.push_back(PhasePointContainer(trailRenderer,phaseTrajectory));
			}

	simulationStartTime = 0;
	simulationEndTime = simulationEndTimeVolatile - simulationStartTimeVolatile;
	simulationTimeStep = simulationTimeStepVolatile;

	simulationTimeCounter = 0;
}

void PhaseFlow::ClearPhasePoints()
{
	for (PhasePointContainer& point : phasePoints)
		point.trail->GetSceneObject()->Destroy();//point.first->GetSceneObject()->Destroy();

	phasePoints.clear();
}

void PhaseFlow::Simulation()
{
	if (simulationTimeCounter > simulationEndTime)
		return;

	float fullTime = simulationEndTime - simulationStartTime;
	for (PhasePointContainer& point : phasePoints)
	{
		size_t firstIndex = simulationTimeCounter / simulationTimeStep; //floor(point.trajectory.size() * simulationTimeCounter / fullTime);
		size_t secondIndex = firstIndex;
		if (secondIndex < point.trajectory.size() - 1)
			secondIndex++;

		double interpolationParameter = (simulationTimeCounter - firstIndex * simulationTimeStep) / simulationTimeStep;
		if (interpolationParameter < 0)
			engine->ConsoleLog("PhaseFlow::Simulation() " + std::to_string(interpolationParameter));

		//Vector<float> newPosition = point.trajectory[firstIndex].second;

		Vector<float> newPosition = point.trajectory[firstIndex].second * (1 - interpolationParameter) + point.trajectory[secondIndex].second * interpolationParameter;

		point.trail->GetSceneObject()->transform.SetPosition(GetNewTrailPosition(newPosition));
	}

	simulationTimeCounter += engine->GetDeltaTime();
}

void PhaseFlow::Start()
{
	CalculatorFunctionSetup();
	PresetEquationsSetup();
	programID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");
	coordinateRenderer = sceneObject->AddBehaviour<CoordinateSystemRenderer>();
	coordinateRenderer->SetRadius(20);
	coordinateRenderer->SetSegmentLength(1);
	coordinateRenderer->SetSegmentNotchSize(0.1);
}

void PhaseFlow::Update()
{
	if (stopSimulation)
	{
		stopSimulation = false;
		ClearPhasePoints();
	}

	if (startSimulation)
	{
		startSimulation = false;
		PhasePointsSetup();
	}

	Simulation();

	/*for (std::pair<TrailRenderer*, std::vector<float> >& phasePoint : phasePoints)
	{
		std::vector<float> phaseSpeed = GetPhaseSpeedVector(phasePoint.second);
		for (size_t i = 0; i < differentialEquation->GetOrder(); ++i)
			phasePoint.second[i] += simulationSpeed * (float)(engine->GetDeltaTime()) * phaseSpeed[i];

		phasePoint.first->GetSceneObject()->transform.SetPosition(GetNewTrailPosition(phasePoint.second));
	}*/

	HandleUI();
}

void PhaseFlow::OnDestroy()
{
	ClearPhasePoints();
	glDeleteProgram(programID);
	delete[] equationInput;
	delete[] variablesInput;
}

void PhaseFlow::SetDifferentialEquation(DifferentialEquation* differentialEquationReference)
{
	delete differentialEquation;
	differentialEquation = differentialEquationReference;
}

void PhaseFlow::HandleUI()
{
	switch (currentUIState)
	{
	case UIState::EquationSetting: EquationSettingUI(); break;
	case UIState::Simulation: SimulationWindowUI(); break;
	case UIState::StartingMenu: StartingMenuUI(); break;

	default:
		break;
	}
}

void PhaseFlow::EquationSettingUI()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("Equation Editing");

	ImGui::InputInt("Equation Order", &diffEqOrder);
	if (diffEqOrder < 1)
		diffEqOrder = 1;
	ImGui::Text("diff(%d) = ", diffEqOrder); ImGui::SameLine();

	ImGui::InputText("", equationInput, 100);
	ImGui::InputTextMultiline("Variables", variablesInput, 100);
	


	if (ImGui::Button("Confirm Equation"))
	{
		std::stringstream tempStream;
		tempStream << equationInput << '\n' << variablesInput;
		
		SetDifferentialEquation(new DifferentialEquation(diffEqOrder, Expression::ReadExpression(tempStream)));

		// validation

		// Setting up coordinate axis
		bool coordY = diffEqOrder >= 2;
		bool coordZ = diffEqOrder >= 3;
		coordinateRenderer->SetAxis(true, coordY, coordZ);
		stopSimulation = true;
		currentUIState = UIState::Simulation;
	}

	if (ImGui::Button("Starting Menu"))
		currentUIState = UIState::StartingMenu;

	ImGui::End();
}

void PhaseFlow::SimulationWindowUI()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("Simulation Controls");

	ImGui::Text("%.1f FPS (%.3f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("Simulation Time: %.3f sec", simulationTimeCounter);
	ImGui::Text("Differential Equation:");

	std::string expression = "diff(" + std::to_string(differentialEquation->GetOrder()) + ") = " + differentialEquation->rightSideExpression.GetString().c_str();
	ImGui::Text(expression.c_str());
	if (equationErrorMessage != "")
		ImGui::Text(("Error: " + equationErrorMessage).c_str());

	if (ImGui::Button("Edit Equation"))
		currentUIState = UIState::EquationSetting;

	if (ImGui::Button("Starting Menu"))
		currentUIState = UIState::StartingMenu;

	int sampleCount = sampleSize;
	ImGui::InputInt("Sample Count", &sampleCount);
	if (sampleCount < 0)
		sampleCount = 0;
	sampleSize = sampleCount;

	ImGui::InputFloat("Simulation Radius", &simulationRadius);
	if (simulationRadius < 0.01)
		simulationRadius = 0.01;

	ImGui::Text("");
	ImGui::InputFloat("Start time", &simulationStartTimeVolatile);
	ImGui::InputFloat("End time", &simulationEndTimeVolatile);
	if (simulationEndTimeVolatile < simulationStartTimeVolatile)
		simulationEndTimeVolatile = simulationStartTimeVolatile;

	ImGui::InputFloat("Simulation time step", &simulationTimeStepVolatile);
	if (simulationTimeStepVolatile <= 0)
		simulationTimeStepVolatile = 0.001;

	if (ImGui::Button("Start"))
		startSimulation = true;

	if (ImGui::Button("Stop"))
		stopSimulation = true;

	static float timeScale = 1;
	ImGui::SliderFloat("Simulation speed", &timeScale, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
	engine->timeScale = timeScale;


	ImGui::Text("\nCoordinates:");

	ImGui::Checkbox("Show coordinates", &(coordinateRenderer->enabled));

	static float coordinateRadius = 20;
	static float segmentLength = 1;
	static float segmentNotchSize = 0.1;

	if (coordinateRenderer->enabled)
	{
		if (ImGui::InputFloat("Radius", &coordinateRadius))
			coordinateRenderer->SetRadius(coordinateRadius);


		if (ImGui::InputFloat("Segment length", &segmentLength))
			coordinateRenderer->SetSegmentLength(segmentLength);


		if (ImGui::InputFloat("Segment notch size", &segmentNotchSize))
			coordinateRenderer->SetSegmentNotchSize(segmentNotchSize);
	}
	ImGui::End();
}

void PhaseFlow::StartingMenuUI()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("Starting Menu");

	if (ImGui::Button("Custom Equation"))
	{
		delete[] equationInput;
		delete[] variablesInput;
		equationInput = new char[100]();
		variablesInput = new char[100]();
		diffEqOrder = 1;
		currentUIState = UIState::EquationSetting;
	}

	for (DifferentialEquationInput& input : presetEquations)
	{
		if (ImGui::Button(input.equationName.c_str()))
		{
			delete[] equationInput;
			delete[] variablesInput;
			equationInput = new char[100]();
			variablesInput = new char[100]();

			diffEqOrder = input.order;
			for (size_t i = 0; i < input.equationInput.size(); ++i)
				equationInput[i] = input.equationInput[i];

			for (size_t i = 0; i < input.variablesInput.size(); ++i)
				variablesInput[i] = input.variablesInput[i];
			currentUIState = UIState::EquationSetting;
		}
	}

	ImGui::End();
}
