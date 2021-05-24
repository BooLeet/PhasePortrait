#include "PhasePortrait.h"
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
#include "ColorUtility.h"
#include "Engine/Utility.h"
#include "Engine/CameraBehaviour.h"
#include "Engine/SphericalCoordinateBehaviour.h"

#include "Vector.h"


#include <string>
#include <fstream>
#include <strstream>
#include <functional>
#include <charconv>

using namespace std;

#pragma region Setters

void PhasePortrait::CalculatorFunctionSetup()
{
	calculatorFunctions.insert(make_pair("+", [](stack<double>& args) {return StackPopReturn(args) + StackPopReturn(args); }));
	calculatorFunctions.insert(make_pair("-", [](stack<double>& args) {return -StackPopReturn(args) + StackPopReturn(args); }));
	calculatorFunctions.insert(make_pair("*", [](stack<double>& args) {return StackPopReturn(args) * StackPopReturn(args); }));
	calculatorFunctions.insert(make_pair("/", [](stack<double>& args) {return 1 / StackPopReturn(args) * StackPopReturn(args); }));
	calculatorFunctions.insert(make_pair("^", [](stack<double>& args) {double power = StackPopReturn(args);  return std::pow(StackPopReturn(args),power); }));

	calculatorFunctions.insert(make_pair("sin", [](stack<double>& args) {return sin(StackPopReturn(args)); }));
	calculatorFunctions.insert(make_pair("cos", [](stack<double>& args) {return cos(StackPopReturn(args)); }));
	calculatorFunctions.insert(make_pair("tan", [](stack<double>& args) {return tan(StackPopReturn(args)); }));

	calculatorFunctions.insert(make_pair("asin", [](stack<double>& args) {return asin(StackPopReturn(args)); }));
	calculatorFunctions.insert(make_pair("acos", [](stack<double>& args) {return acos(StackPopReturn(args)); }));
	calculatorFunctions.insert(make_pair("atan", [](stack<double>& args) {return atan(StackPopReturn(args)); }));

	calculatorFunctions.insert(make_pair("sqrt", [](stack<double>& args) {return sqrt(StackPopReturn(args)); }));

	calculatorFunctions.insert(make_pair("sign", [](stack<double>& args) {double val = StackPopReturn(args); return val / abs(val); }));
	calculatorFunctions.insert(make_pair("abs", [](stack<double>& args) {return abs(StackPopReturn(args)); }));
	calculatorFunctions.insert(make_pair("log", [](stack<double>& args) {return log(StackPopReturn(args)); }));

	calculatorFunctions.insert(make_pair("diff", [this](std::stack<double>& args) {return currentPointForCalculation[StackPopReturn(args)]; }));
	calculatorFunctions.insert(make_pair("var", [this](std::stack<double>& args) {return currentPointForCalculation[StackPopReturn(args)]; }));

}

void PhasePortrait::PresetsSetup()
{
	presetEquations.push_back(DifferentialEquationPreset(2, "(0-mu) * diff(1) - (g / L) * sin(diff(0))", "mu = 0.5\ng = 10\nL = 5", "Pendulum Equation"));
	presetEquations.push_back(DifferentialEquationPreset(2, "mu*(1-diff(0)*diff(0))*diff(1)-diff(0)", "mu = 0.1", "Van der Pol oscillator"));

	presetSystems.push_back(SystemPreset({	"sigma * (var(1) - var(0))",
											"var(0) * (rho - var(2)) - var(1)", 
											"var(0) * var(1) - beta * var(2)" },
		"sigma = 10\nrho = 28\nbeta = 2.666666", "Lorenz Attractor"));
	
	presetSystems.push_back(SystemPreset({ "var(1)-a*var(0)+b*var(1)*var(2)",
											"c*var(1)-var(0)*var(2)+var(2)",
											"d*var(0)*var(1)-e*var(2)" },
		"a = 3\nb = 2.7\nc = 1.7\nd = 2\ne = 9", "Dadras Attractor"));

	presetSystems.push_back(SystemPreset({ "0-a*var(0)-4*var(1)-4*var(2)-var(1)^2",
											"0-a*var(1)-4*var(2)-4*var(0)-var(2)^2",
											"0-a*var(2)-4*var(0)-4*var(1)-var(0)^2" },
		"a = 1.89", "Halvorsen Attractor"));

	presetSystems.push_back(SystemPreset({ "var(1)+a*var(0)*var(1)+var(0)*var(2)",
											"1-b*var(0)^2+var(1)*var(2)",
											"var(0)-var(0)^2-var(1)^2" },
		"a = 2.07\nb = 1.79", "Sprott Attractor"));

	presetSystems.push_back(SystemPreset({ "a*var(0)+var(1)*var(2)",
											"b*var(0)-c*var(1)-var(0)*var(2)",
											"0-var(2)-var(0)*var(1)" },
		"a = 0.2\nb = 0.01\nc = 0.4", "Four-Wing Attractor"));

	presetSystems.push_back(SystemPreset({ "sin(var(1))-b*var(0)",
											"sin(var(2))-b*var(1)",
											"sin(var(0))-b*var(2)" },
		"b = 0.208186", "Thomas Attractor"));
}

void PhasePortrait::SetDifferentialEquation(DifferentialEquation* differentialEquationReference)
{
	delete differentialEquation;
	differentialEquation = differentialEquationReference;
}

void PhasePortrait::SetSystemOfODEs(const std::vector<std::vector<char>>& system, char* variables)
{
	systemOfODEs.clear();
	systemOfODEs.reserve(system.size());

	for (size_t i = 0; i < system.size(); ++i)
	{
		std::stringstream tempStream;
		for (size_t j = 0; j < system[i].size(); ++j)
			if(system[i][j] != 0)
				tempStream << system[i][j];
		tempStream << '\n' << variablesInput;
		systemOfODEs.push_back(Expression::ReadExpression(tempStream));
	}
}

#pragma endregion

#pragma region Phase Trajectory Creation

float PhasePortrait::CalculateDifferentialEquation(double t, const VectorWrap<float>& point)
{
	currentPointForCalculation = point;
	differentialEquation->rightSideExpression.SetVariable("t", t);
	std::pair<double, std::string> result = differentialEquation->rightSideExpression.EvaluateExpressionAndGetError(calculatorFunctions);
	equationErrorMessage = result.second;
	if (result.second == "")
		return result.first;

	stopSimulation = true;
	return 0;
}

vector< VectorWrap<float>> PhasePortrait::SolveRungeKutta(function<VectorWrap<float>(double,const VectorWrap<float>&)> f, const vector<float>& yStart, double tStart, double tEnd, double tStep)
{
	if (tStart > tEnd)
		std::swap(tStart, tEnd);

	size_t steps = std::ceil((tEnd - tStart) / tStep);

	double tPrev = tStart;
	VectorWrap<float> yPrev = yStart;
	std::vector< VectorWrap<float>> result;
	result.push_back(yPrev);

	for (size_t i = 1; i <= steps; ++i)
	{
		VectorWrap<float> K1 = tStep * f(tPrev, yPrev);
		VectorWrap<float> K2 = tStep * f(tPrev + tStep / 2, yPrev + K1 * 0.5);
		VectorWrap<float> K3 = tStep * f(tPrev + tStep / 2, yPrev + K2 * 0.5);
		VectorWrap<float> K4 = tStep * f(tPrev + tStep, yPrev + K3);

		yPrev = yPrev + (K1 + 2 * K2 + 2 * K3 + K4) * (1 / 6.0);
		tPrev += tStep;
		result.push_back(yPrev);
	}

	return result;
}

void PhasePortrait::CreatePhaseTrajectories()
{
	// Удаление предыдущих точек
	ClearPhaseTrajectories();

	// Порядок уравнения / размер системы
	size_t order = (differentialEquation == nullptr) ? systemOfODEs.size() : differentialEquation->GetOrder();

	// Определение количества итераций циклов
	size_t pointsPerDimension = sampleSize;
	if (order == 2)
		pointsPerDimension = sqrt(sampleSize);
	else if (order >= 3)
		pointsPerDimension = cbrt(sampleSize);

	size_t iSize = order >= 1 ? pointsPerDimension : 0;
	size_t jSize = order >= 2 ? pointsPerDimension : 1;
	size_t kSize = order >= 3 ? pointsPerDimension : 1;

	// Вектор-функция фазовой скорости
	function<VectorWrap<float>(double, const VectorWrap<float>&)> F;
	if (differentialEquation != nullptr)
		F = [this](double t, const VectorWrap<float>& point)
		{
			std::vector<float> result(differentialEquation->GetOrder());

			for (size_t i = 0; i < differentialEquation->GetOrder() - 1; ++i)
				result[i] = point[i + 1];

			result[differentialEquation->GetOrder() - 1] = CalculateDifferentialEquation(t, point);

			return result;
		};
	else
		F = [this](double t, const VectorWrap<float>& point)
		{
			vector<float> result(systemOfODEs.size());
			currentPointForCalculation = point;
			for (size_t i = 0; i < systemOfODEs.size(); ++i)
			{
				systemOfODEs[i].SetVariable("t", t);
				result[i] = systemOfODEs[i].EvaluateExpression(calculatorFunctions);
			}

			return result;
		};

	// Создание начальных точек и вычисление фазовых траекторий
	for (size_t i = 0; i < iSize; ++i)
		for (size_t j = 0; j < jSize; ++j)
			for (size_t k = 0; k < kSize; ++k)
			{
				// Создание новой точки и заполнение значений переменных/производных, не уместившихся на координатных осях
				vector<float> phasePosition(sliceValues);

				// Заполнение координат в виде квадрата / куба с центром в точке 0
				if (order > xDiffOrder)
					phasePosition[xDiffOrder] = 2 * simulationRadius * (i / (float)(pointsPerDimension - 1) - 0.5);

				if (order > yDiffOrder)
					phasePosition[yDiffOrder] = 2 * simulationRadius * (j / (float)(pointsPerDimension - 1) - 0.5);

				if (order > zDiffOrder)
					phasePosition[zDiffOrder] = 2 * simulationRadius * (k / (float)(pointsPerDimension - 1) - 0.5);

				// вычисление фазовых траекторий методом Рунге Кутта
				vector< VectorWrap<float>> phaseTrajectory = SolveRungeKutta(F, phasePosition, simulationStartTimeVolatile, simulationEndTimeVolatile, simulationTimeStepVolatile);
				phaseTrajectories.push_back(phaseTrajectory);
			}

	simulationStartTime = 0;
	simulationEndTime = simulationEndTimeVolatile - simulationStartTimeVolatile;
	simulationTimeStep = simulationTimeStepVolatile;

	simulationTimeCounter = 0;
}

void PhasePortrait::ClearPhaseTrajectories()
{
	phaseTrajectories.clear();
}

#pragma endregion

#pragma region Rendering

vec3 PhasePortrait::GetTrajectoryColor(float parameter)
{
	switch (currentColorMode)
	{
	case TrajectoryColorMode::SingleColor: return trajectoryFirstColor;
	case TrajectoryColorMode::TwoColor: return GetInterpolatedColor(parameter, trajectoryFirstColor, trajectorySecondColor);
	case TrajectoryColorMode::Rainbow: return GetColorRainbow(parameter);
	default:
		break;
	}
}

vec3 PhasePortrait::GetTrailPosition(const VectorWrap<float>& phasePosition)
{
	size_t order = (differentialEquation == nullptr) ? systemOfODEs.size() : differentialEquation->GetOrder();
	vec3 result = vec3(0);
	if (order > xDiffOrder)
		result.x = phasePosition[xDiffOrder];

	if (order > yDiffOrder)
		result.y = phasePosition[yDiffOrder];

	if (order > zDiffOrder)
		result.z = phasePosition[zDiffOrder];

	return result;
}

void PhasePortrait::Render(mat4 projectionViewMatrix)
{
	float fullTime = simulationEndTime - simulationStartTime;
	for (std::vector< VectorWrap<float>>& trajectory : phaseTrajectories)
	{
		std::vector<GLfloat> vertexData;
		std::vector<GLfloat> colorData;

		size_t firstIndex = simulationTimeCounter / simulationTimeStep;
		size_t secondIndex = firstIndex;
		if (secondIndex < trajectory.size() - 1)
			secondIndex++;

		double interpolationParameter = (simulationTimeCounter - firstIndex * simulationTimeStep) / simulationTimeStep;

		// Creating a head of the trajectory
		vec3 headPosition = GetTrailPosition(trajectory[firstIndex] * (1 - interpolationParameter) + trajectory[secondIndex] * interpolationParameter);

		vertexData.push_back(headPosition.x);
		vertexData.push_back(headPosition.y);
		vertexData.push_back(headPosition.z);

		colorData.push_back(trajectoryFirstColor.r);
		colorData.push_back(trajectoryFirstColor.g);
		colorData.push_back(trajectoryFirstColor.b);

		// Creating the rest of the trajectory
		vec3 previousPosition = headPosition;
		float currentLength = 0;
		bool endTrajectory = false;
		float currentEdgeLength = 0;

		for (int i = firstIndex; i >= 0 && !endTrajectory; --i)
		{
			// Position
			vec3 position = GetTrailPosition(trajectory[i]);
			float currentDistance = distance(previousPosition, position);
			
			if (currentLength + currentDistance >= trajectoryMaxLength)
			{
				endTrajectory = true;
				float positionInterpolator = (trajectoryMaxLength - currentLength) / currentDistance;

				position = (1 - positionInterpolator) * previousPosition + positionInterpolator * position;
				currentDistance = trajectoryMaxLength - currentLength;
			}
			

			currentEdgeLength += currentDistance;
			if (currentEdgeLength < trajectoryMinEdgeLength)
				continue;
			currentEdgeLength = 0;

			vertexData.push_back(position.x);
			vertexData.push_back(position.y);
			vertexData.push_back(position.z);

			previousPosition = position;
			currentLength += currentDistance;

			// Color
			float colorParameter = currentLength / trajectoryMaxLength;
			vec3 newColor = GetTrajectoryColor(colorParameter);

			colorData.push_back(newColor.r);
			colorData.push_back(newColor.g);
			colorData.push_back(newColor.b);
		}
		Mesh meshToDraw = Mesh(vertexData, colorData, engine->GetDefaultShader().GetProgram(), GL_LINE_STRIP);
		meshToDraw.Render(projectionViewMatrix, sceneObject->GetTransform().GetModelMatrix());
	}
}

#pragma endregion

void PhasePortrait::Start()
{
	CalculatorFunctionSetup();
	PresetsSetup();
	coordinateRenderer = sceneObject->AddBehaviour<CoordinateSystemRenderer>();
	coordinateRenderer->SetRadius(20);
	coordinateRenderer->SetSegmentLength(1);
	coordinateRenderer->SetSegmentNotchSize(0.1);


	SceneObject* camera = engine->GetScene()->CreateObject("Camera");
	CameraBehaviour* camBehaviour = camera->AddBehaviour<CameraBehaviour>();
	camBehaviour->farPlane = 5000;
	cameraHolder = camera->AddBehaviour<SphericalCoordinateBehaviour>();
	camera->GetTransform().SetPosition(vec3(0, 0, 0));
	
	cameraHolder->distance = 10;
}

void PhasePortrait::Update()
{
	if (stopSimulation)
	{
		stopSimulation = false;
		ClearPhaseTrajectories();
	}

	if (startSimulation)
	{
		startSimulation = false;
		CreatePhaseTrajectories();
	}

	simulationTimeCounter += engine->GetDeltaTime();

	if (simulationTimeCounter > simulationEndTime)
		simulationTimeCounter = simulationEndTime;
	
	HandleUI();
}

void PhasePortrait::OnDestroyRenderer()
{
	ClearPhaseTrajectories();
	delete[] equationInput;
	delete[] variablesInput;
	cameraHolder->GetSceneObject()->Destroy();
}

#pragma region UI

void PhasePortrait::HandleUI()
{
	switch (currentUIState)
	{
	case UIState::EquationSetting: EquationSettingUI(); break;
	case UIState::SystemSetting: SystemSettingUI(); break;
	case UIState::Simulation: SimulationWindowUI(); break;
	case UIState::StartingMenu: StartingMenuUI(); break;

	default:
		break;
	}
}

void PhasePortrait::EquationSettingUI()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowContentWidth(engine->GetWindowWidth() / 3);
	ImGui::Begin("Equation Editing");

	if (ImGui::InputInt("Equation Order", &diffEqOrder))
	{
		if (diffEqOrder < 1)
			diffEqOrder = 1;
		sliceValues = std::vector<float>(diffEqOrder);
	}
	
	ImGui::Text("diff(%d) = ", diffEqOrder); ImGui::SameLine();

	ImGui::InputText("", equationInput, 100);
	ImGui::InputTextMultiline("Constants", variablesInput, 100);
	
	ImGui::Spacing();

	if (diffEqOrder > 1)
	{
		int xOrder = xDiffOrder;
		ImGui::InputInt("x diff order", &xOrder);
		xDiffOrder = Clamp<int>(xOrder, 0, diffEqOrder - 1);

		int yOrder = yDiffOrder;
		ImGui::InputInt("y diff order", &yOrder);
		yDiffOrder = Clamp<int>(yOrder, 0, diffEqOrder - 1);
	}

	if (diffEqOrder > 2)
	{
		int zOrder = zDiffOrder;
		ImGui::InputInt("z diff order", &zOrder);
		zDiffOrder = Clamp<int>(zOrder, 0, diffEqOrder - 1);
	}

	if (diffEqOrder > 3)
	{
		for (size_t i = 0; i < diffEqOrder; ++i)
		{
			if (i != xDiffOrder && i != yDiffOrder && i != zDiffOrder)
				ImGui::InputFloat((to_string(i) + " order derivative value").c_str(), &(sliceValues[i]));
		}
	}

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

void PhasePortrait::SystemSettingUI()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowContentWidth(engine->GetWindowWidth() / 3);
	ImGui::Begin("System Editing");

	if (ImGui::InputInt("System size", &systemSize))
	{
		if (systemSize < 1)
			systemSize = 1;
		sliceValues = vector<float>(systemSize);
		systemInput = vector<vector<char>>(systemSize, vector<char>(100));
	}

	for (int i = 0; i < systemSize; ++i)
	{
		ImGui::Text("d(var(%d))/dt = ", i); ImGui::SameLine();
		ImGui::InputText(std::to_string(i).c_str(), &(systemInput[i][0]), 100);
	}

	ImGui::InputTextMultiline("Constants", variablesInput, 100);

	ImGui::Spacing();

	if (systemSize > 1)
	{
		int xOrder = xDiffOrder;
		ImGui::InputInt("x var index", &xOrder);
		xDiffOrder = Clamp<int>(xOrder, 0, systemSize - 1);

		int yOrder = yDiffOrder;
		ImGui::InputInt("y var index", &yOrder);
		yDiffOrder = Clamp<int>(yOrder, 0, systemSize - 1);
	}

	if (systemSize > 2)
	{
		int zOrder = zDiffOrder;
		ImGui::InputInt("z var index", &zOrder);
		zDiffOrder = Clamp<int>(zOrder, 0, systemSize - 1);
	}

	if (systemSize > 3)
	{
		for (size_t i = 0; i < systemSize; ++i)
			if (i != xDiffOrder && i != yDiffOrder && i != zDiffOrder)
				ImGui::InputFloat(("var(" + to_string(i) + ") value").c_str(), &(sliceValues[i]));
	}

	if (ImGui::Button("Confirm System"))
	{
		std::stringstream tempStream;
		tempStream << equationInput << '\n' << variablesInput;

		SetDifferentialEquation(nullptr);
		SetSystemOfODEs(systemInput, variablesInput);

		// Setting up coordinate axis
		bool coordY = systemSize >= 2;
		bool coordZ = systemSize >= 3;
		coordinateRenderer->SetAxis(true, coordY, coordZ);
		stopSimulation = true;
		currentUIState = UIState::Simulation;
	}

	if (ImGui::Button("Starting Menu"))
		currentUIState = UIState::StartingMenu;

	ImGui::End();
}

void PhasePortrait::SimulationWindowUI()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowContentWidth(engine->GetWindowWidth() / 3);
	ImGui::Begin("Simulation Controls",nullptr,ImGuiWindowFlags_NoSavedSettings);

	ImGui::Text("%.1f FPS (%.3f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("Differential Equation:");

	//std::string expression = "diff(" + std::to_string(differentialEquation->GetOrder()) + ") = " + differentialEquation->rightSideExpression.GetString().c_str();
	//ImGui::Text(expression.c_str());
	if (equationErrorMessage != "")
		ImGui::Text(("Error: " + equationErrorMessage).c_str());

	if ((differentialEquation == nullptr))
	{
		if (ImGui::Button("Edit System"))
		{
			stopSimulation = true;
			currentUIState = UIState::SystemSetting;
		}
	}
	else
	{
		if (ImGui::Button("Edit Equation"))
		{
			stopSimulation = true;
			currentUIState = UIState::EquationSetting;
		}
	}
		
	if (ImGui::Button("Starting Menu"))
	{
		stopSimulation = true;
		currentUIState = UIState::StartingMenu;
	}

	ImGui::Spacing();

	// PLAYER
	if (ImGui::Button("Create portrait"))
		startSimulation = true;

	if (ImGui::Button("Clear portrait"))
		stopSimulation = true;

	if (ImGui::Button("Play"))
	{
		if (phaseTrajectories.size() == 0)
			startSimulation = true;
		engine->timeScale = 1;
	}

	if (ImGui::Button("Pause"))
		engine->timeScale = 0;

	ImGui::Spacing();

	float timeScale = engine->timeScale;
	ImGui::SliderFloat("Simulation speed", &timeScale, 0.0f, 10.0f);
	engine->timeScale = timeScale;

	ImGui::SliderFloat("Simulation time", &simulationTimeCounter, simulationStartTime, simulationEndTime);

	// SIMULATION
	if (ImGui::CollapsingHeader("Simulation menu"))
	{
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

		ImGui::Separator();
	}

	// TRAJECTORY
	ImGui::Spacing();
	if (ImGui::CollapsingHeader("Trajectory settings"))
	{
		ImGui::InputFloat("Max length", &trajectoryMaxLength);
		if (trajectoryMaxLength <= 0)
			trajectoryMaxLength = 0.01;

		ImGui::InputFloat("Min edge length", &trajectoryMinEdgeLength);
		if (trajectoryMinEdgeLength <= 0)
			trajectoryMinEdgeLength = 0;
		
		if (ImGui::CollapsingHeader("Background color"))
			ImGui::ColorPicker3("Background color", &engine->clearColor[0]);

		const char* colorModes[] = { "Single Color", "Two Color", "Rainbow"};

		if (ImGui::Button("Select color mode"))
			ImGui::OpenPopup("select");
		ImGui::SameLine();
		ImGui::TextUnformatted(colorModes[(int)currentColorMode]);
		if (ImGui::BeginPopup("select"))
		{
			for (int i = 0; i < IM_ARRAYSIZE(colorModes); i++)
				if (ImGui::Selectable(colorModes[i]))
					currentColorMode = (TrajectoryColorMode)i;
			ImGui::EndPopup();
		}

		if (currentColorMode == TrajectoryColorMode::SingleColor)
		{
			ImGui::ColorPicker3("Color", &trajectoryFirstColor[0]);
		}

		if (currentColorMode == TrajectoryColorMode::TwoColor)
		{
			ImGui::ColorPicker3("Head color", &trajectoryFirstColor[0]);
			ImGui::ColorPicker3("Tail color", &trajectorySecondColor[0]);
		}
			
		ImGui::Separator();
	}
	
	// COORDINATES
	ImGui::Spacing();
	ImGui::Checkbox("Show coordinates", &(coordinateRenderer->enabled));

	static float coordinateRadius = 20;
	static float segmentLength = 1;
	static float segmentNotchSize = 0.1;

	if (coordinateRenderer->enabled)
		if (ImGui::CollapsingHeader("Coordinates settings"))
		{
			if (ImGui::InputFloat("Radius", &coordinateRadius))
				coordinateRenderer->SetRadius(coordinateRadius);


			if (ImGui::InputFloat("Segment length", &segmentLength))
				coordinateRenderer->SetSegmentLength(segmentLength);


			if (ImGui::InputFloat("Segment notch size", &segmentNotchSize))
				coordinateRenderer->SetSegmentNotchSize(segmentNotchSize);

			ImGui::Spacing();

			vec3 xColor = coordinateRenderer->GetColorX();
			vec3 yColor = coordinateRenderer->GetColorY();
			vec3 zColor = coordinateRenderer->GetColorZ();

			if (ImGui::ColorPicker3("Color X", &xColor[0]))
				coordinateRenderer->SetColorX(xColor);
			
			if (ImGui::ColorPicker3("Color Y", &yColor[0]))
				coordinateRenderer->SetColorY(yColor);

			if (ImGui::ColorPicker3("Color Z", &zColor[0]))
				coordinateRenderer->SetColorZ(zColor);

			ImGui::Separator();
		}
	
	// CAMERA HOLDER INPUT
	ImGui::Spacing();
	if (ImGui::CollapsingHeader("Camera settings"))
	{
		if (ImGui::Button("Reset camera"))
			cameraHolder->Reset(10);

		ImGui::InputFloat3("Pivot", &(cameraHolder->pivot[0]));
		vec3 rotation = (180 / pi<float>()) * vec3(cameraHolder->angleU, cameraHolder->angleV, cameraHolder->angleW);
		if (ImGui::InputFloat3("Rotation", &(rotation[0])))
		{
			rotation = (pi<float>() / 180) * rotation;
			cameraHolder->angleU = rotation.x;
			cameraHolder->angleV = rotation.y;
			cameraHolder->angleW = rotation.z;
		}


		ImGui::Separator();
	}
	cameraHolder->HandleInput();

	ImGui::End();
}

void PhasePortrait::StartingMenuUI()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowContentWidth(engine->GetWindowWidth() / 3);
	ImGui::Begin("Starting Menu");

	if (ImGui::Button("Custom Equation"))
	{
		delete[] equationInput;
		delete[] variablesInput;
		equationInput = new char[100]();
		variablesInput = new char[100]();
		diffEqOrder = 1;
		sliceValues = vector<float>(diffEqOrder);
		currentUIState = UIState::EquationSetting;
	}

	for (DifferentialEquationPreset& preset : presetEquations)
	{
		if (ImGui::Button(preset.equationName.c_str()))
		{
			delete[] equationInput;
			delete[] variablesInput;
			equationInput = new char[100]();
			variablesInput = new char[100]();

			diffEqOrder = preset.order;
			sliceValues = std::vector<float>(diffEqOrder, 0);
			for (size_t i = 0; i < preset.equationInput.size(); ++i)
				equationInput[i] = preset.equationInput[i];

			for (size_t i = 0; i < preset.variablesInput.size(); ++i)
				variablesInput[i] = preset.variablesInput[i];
			currentUIState = UIState::EquationSetting;
		}
	}

	ImGui::Separator();

	if (ImGui::Button("Custom System of ODEs"))
	{
		systemSize = 3;

		sliceValues = vector<float>(systemSize);
		systemInput = std::vector<std::vector<char>>(systemSize, vector<char>(100));

		currentUIState = UIState::SystemSetting;
	}

	for (SystemPreset& preset : presetSystems)
	{
		if (ImGui::Button(preset.systemName.c_str()))
		{
			systemSize = preset.systemInput.size();
			systemInput = preset.systemInput;
			
			variablesInput = new char[100]();
			sliceValues = std::vector<float>(systemInput.size(), 0);

			delete[] variablesInput;
			for (size_t i = 0; i < preset.variablesInput.size(); ++i)
				variablesInput[i] = preset.variablesInput[i];
			currentUIState = UIState::SystemSetting;
		}
	}

	ImGui::Spacing();
	if (ImGui::Button("Quit"))
		engine->Quit();

	ImGui::End();
}

#pragma endregion
