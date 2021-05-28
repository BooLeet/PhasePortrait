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
#include "PhasePortraitStates.h"

#include "Vector.h"


#include <string>
#include <fstream>
#include <strstream>
#include <functional>
#include <charconv>

using namespace std;
using namespace PhasePortraitStates;

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


#pragma endregion

#pragma region Phase Trajectory Creation

float PhasePortrait::CalculateDifferentialEquation(DifferentialEquation* equation, double t, const VectorWrap<float>& point)
{
	currentPointForCalculation = point;
	equation->rightSideExpression.SetVariable("t", t);
	std::pair<double, std::string> result = equation->rightSideExpression.EvaluateExpressionAndGetError(calculatorFunctions);
	if (result.second == "")
		return result.first;

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

void PhasePortrait::CreatePhaseTrajectories(function<VectorWrap<float>(double, const VectorWrap<float>&)> speedFunction,size_t dimensionCount,size_t sampleSize, float simulationRadius, float startTime, float endTime,float timeStep,size_t xOrder,size_t yOrder, size_t zOrder,const vector<float>& sliceValues)
{
	// Удаление предыдущих точек
	ClearPhaseTrajectories();

	// Определение количества итераций циклов
	size_t pointsPerDimension = sampleSize;
	if (dimensionCount == 2)
		pointsPerDimension = sqrt(sampleSize);
	else if (dimensionCount >= 3)
		pointsPerDimension = cbrt(sampleSize);

	size_t iSize = dimensionCount >= 1 ? pointsPerDimension : 0;
	size_t jSize = dimensionCount >= 2 ? pointsPerDimension : 1;
	size_t kSize = dimensionCount >= 3 ? pointsPerDimension : 1;


	// Создание начальных точек и вычисление фазовых траекторий
	for (size_t i = 0; i < iSize; ++i)
		for (size_t j = 0; j < jSize; ++j)
			for (size_t k = 0; k < kSize; ++k)
			{
				// Создание новой точки и заполнение значений переменных/производных, не уместившихся на координатных осях
				vector<float> phasePosition(sliceValues);

				// Заполнение координат в виде квадрата / куба с центром в точке 0
				if (dimensionCount > xOrder)
					phasePosition[xOrder] = 2 * simulationRadius * (i / (float)(pointsPerDimension - 1) - 0.5);

				if (dimensionCount > yOrder)
					phasePosition[yOrder] = 2 * simulationRadius * (j / (float)(pointsPerDimension - 1) - 0.5);

				if (dimensionCount > zOrder)
					phasePosition[zOrder] = 2 * simulationRadius * (k / (float)(pointsPerDimension - 1) - 0.5);

				// вычисление фазовых траекторий методом Рунге Кутта
				vector< VectorWrap<float>> fullPhaseTrajectory = SolveRungeKutta(speedFunction, phasePosition, startTime, endTime, timeStep);
				vector<vec3> visiblePhaseTrajectory;
				for (size_t indx = 0; indx < fullPhaseTrajectory.size(); ++indx)
				{
					vec3 point(0, 0, 0);
					if (dimensionCount > xOrder)
						point.x = fullPhaseTrajectory[indx][xOrder];

					if (dimensionCount > yOrder)
						point.y = fullPhaseTrajectory[indx][yOrder];

					if (dimensionCount > zOrder)
						point.z = fullPhaseTrajectory[indx][zOrder];
					visiblePhaseTrajectory.push_back(point);
				}
				phaseTrajectories.push_back(visiblePhaseTrajectory);
			}

	simulationStartTime = 0;
	simulationEndTime = endTime - startTime;
	simulationTimeStep = timeStep;
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
	switch (trajectorySettings.currentColorMode)
	{
	case TrajectorySettings::ColorMode::SingleColor: return trajectorySettings.firstColor;
	case TrajectorySettings::ColorMode::TwoColor: return GetInterpolatedColor(parameter, trajectorySettings.firstColor, trajectorySettings.secondColor);
	case TrajectorySettings::ColorMode::Rainbow: return GetColorRainbow(parameter);
	default:
		break;
	}
}

void PhasePortrait::Render(mat4 projectionViewMatrix)
{
	float fullTime = simulationEndTime - simulationStartTime;
	for (std::vector< vec3>& trajectory : phaseTrajectories)
	{
		std::vector<GLfloat> vertexData;
		std::vector<GLfloat> colorData;

		size_t firstIndex = simulationTimeCounter / simulationTimeStep;
		size_t secondIndex = firstIndex;
		if (secondIndex < trajectory.size() - 1)
			secondIndex++;

		float interpolationParameter = (simulationTimeCounter - firstIndex * simulationTimeStep) / simulationTimeStep;

		// Creating a head of the trajectory
		vec3 headPosition = trajectory[firstIndex] * (1 - interpolationParameter) + trajectory[secondIndex] * interpolationParameter;

		vertexData.push_back(headPosition.x);
		vertexData.push_back(headPosition.y);
		vertexData.push_back(headPosition.z);

		colorData.push_back(trajectorySettings.firstColor.r);
		colorData.push_back(trajectorySettings.firstColor.g);
		colorData.push_back(trajectorySettings.firstColor.b);

		// Creating the rest of the trajectory
		vec3 previousPosition = headPosition;
		float currentLength = 0;
		bool endTrajectory = false;
		float currentEdgeLength = 0;

		for (int i = firstIndex; i >= 0 && !endTrajectory; --i)
		{
			// Position
			vec3 position = trajectory[i];
			float currentDistance = distance(previousPosition, position);
			
			if (currentLength + currentDistance >= trajectorySettings.maxLength)
			{
				endTrajectory = true;
				float positionInterpolator = (trajectorySettings.maxLength - currentLength) / currentDistance;

				position = (1 - positionInterpolator) * previousPosition + positionInterpolator * position;
				currentDistance = trajectorySettings.maxLength - currentLength;
			}
			

			currentEdgeLength += currentDistance;
			if (currentEdgeLength < trajectorySettings.minEdgeLength)
				continue;
			currentEdgeLength = 0;

			vertexData.push_back(position.x);
			vertexData.push_back(position.y);
			vertexData.push_back(position.z);

			previousPosition = position;
			currentLength += currentDistance;

			// Color
			float colorParameter = currentLength / trajectorySettings.maxLength;
			vec3 newColor = GetTrajectoryColor(colorParameter);

			colorData.push_back(newColor.r);
			colorData.push_back(newColor.g);
			colorData.push_back(newColor.b);
		}

		Mesh meshToDraw = Mesh(vertexData, colorData, GetEngine()->GetDefaultShader().GetProgram(), GL_LINE_STRIP);
		meshToDraw.Render(projectionViewMatrix, GetSceneObject()->GetTransform().GetModelMatrix());
	}
}

#pragma endregion

void PhasePortrait::Start()
{
	CalculatorFunctionSetup();
	PresetsSetup();
	axisRenderer = GetSceneObject()->AddBehaviour<CoordinateSystemRenderer>();
	axisRenderer->SetRadius(20);
	axisRenderer->SetSegmentLength(1);
	axisRenderer->SetSegmentNotchSize(0.1);


	SceneObject* camera = GetEngine()->GetScene()->CreateObject("Camera");
	CameraBehaviour* camBehaviour = camera->AddBehaviour<CameraBehaviour>();
	camBehaviour->farPlane = 5000;
	cameraHolder = camera->AddBehaviour<SphericalCoordinateBehaviour>();
	camera->GetTransform().SetPosition(vec3(0, 0, 0));
	
	cameraHolder->distance = 10;

	currentState = new StartMenuState();
	currentState->Init(*this);
}

void PhasePortrait::Update()
{
	currentState->Action(*this);
	PhasePortraitState* nextState = currentState->Transition(*this);
	if (nextState != nullptr)
	{
		delete currentState;
		currentState = nextState;
		currentState->Init(*this);
	}

	simulationTimeCounter += GetEngine()->GetDeltaTime();

	if (simulationTimeCounter > simulationEndTime)
		simulationTimeCounter = simulationEndTime;
}

void PhasePortrait::OnDestroyRenderer()
{
	ClearPhaseTrajectories();
	cameraHolder->GetSceneObject()->Destroy();
}

void PhasePortrait::ResetTimeSpeed()
{
	GetEngine()->timeScale = 1;
}

void PhasePortrait::Pause()
{
	GetEngine()->timeScale = 0;
}

function<VectorWrap<float>(double, const VectorWrap<float>&)> PhasePortrait::GetF(DifferentialEquation* equation)
{
	return [this,equation](double t, const VectorWrap<float>& point)
	{
		std::vector<float> result(equation->GetOrder());

		for (size_t i = 0; i < equation->GetOrder() - 1; ++i)
			result[i] = point[i + 1];

		result[equation->GetOrder() - 1] = CalculateDifferentialEquation(equation, t, point);
		return result;
	};
}

function<VectorWrap<float>(double, const VectorWrap<float>&)> PhasePortrait::GetF(std::vector<Expression>* systemOfODEs)
{
	return [this, systemOfODEs](double t, const VectorWrap<float>& point)
	{
		vector<float> result(systemOfODEs->size());
		currentPointForCalculation = point;
		for (size_t i = 0; i < systemOfODEs->size(); ++i)
		{
			(*systemOfODEs)[i].SetVariable("t", t);
			result[i] = (*systemOfODEs)[i].EvaluateExpression(calculatorFunctions);
		}

		return result;
	};
}
