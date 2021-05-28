#pragma once
#include "Engine/RendererBehaviour.h"

#include "ExpressionCalculator.h"
#include "Vector.h"
#include <vector>
#include "Engine/Shader.h"
//#include "PhasePortraitStates.h"

using namespace std;


class DifferentialEquation;
class CoordinateSystemRenderer;
class SphericalCoordinateBehaviour;
class PhasePortraitState;

class PhasePortrait : public RendererBehaviour
{
	PhasePortraitState* currentState;
	
	float simulationTimeStep = 0;
	std::vector<std::vector< vec3>> phaseTrajectories;
	std::map<std::string, calcFunction > calculatorFunctions;
	
	// Вычисляет правую часть дифференциального уравнения для данной фазовой точки
	float CalculateDifferentialEquation(DifferentialEquation* equation, double t, const VectorWrap<float>& point);

	// Вносит необходимые функции для вычисления уравнений
	void CalculatorFunctionSetup();

	// Вносит заранее подготовленные уравнения
	void PresetsSetup();

	// Возвращает значения решения задачи Коши 
	std::vector< VectorWrap<float>> SolveRungeKutta(function<VectorWrap<float>(double, const VectorWrap<float>&)> f, const vector<float>& yStart, double tStart, double tEnd, double tStep);

	vec3 GetTrajectoryColor(float parameter);

public:
	float simulationTimeCounter = 0;

	float simulationStartTime = 0;
	float simulationEndTime = 0;

	class TrajectorySettings
	{
	public:
		float maxLength = 10;
		float minEdgeLength = 0.001;

		enum class ColorMode { SingleColor, TwoColor, Rainbow };
		ColorMode currentColorMode = ColorMode::TwoColor;

		vec3 firstColor = vec3(1, 0, 0);
		vec3 secondColor = vec3(0, 0, 1);
	};
	VectorWrap<float> currentPointForCalculation;

	TrajectorySettings trajectorySettings;

	SphericalCoordinateBehaviour* cameraHolder = nullptr;
	CoordinateSystemRenderer* axisRenderer = nullptr;

	struct DifferentialEquationPreset
	{
		size_t order;
		std::string equationInput;
		std::string constantsInput;
		std::string equationName;

		DifferentialEquationPreset(size_t equationOrder, const std::string& equation, const std::string& variables, const std::string& name) : order(equationOrder), equationInput(equation), constantsInput(variables), equationName(name) {}
	};

	struct SystemPreset
	{
		std::vector<std::vector<char>> systemInput;
		std::string systemName;
		std::string constantsInput;

		SystemPreset(const std::vector<std::string> equationInput, const std::string& constants, const std::string& name) : systemName(name), constantsInput(constants)
		{
			systemInput.reserve(equationInput.size());
			for (size_t i = 0; i < equationInput.size(); ++i)
			{
				systemInput.push_back(std::vector<char>());
				for (size_t j = 0; j < equationInput[i].size(); ++j)
					systemInput[i].push_back(equationInput[i][j]);
			}
		}
	};


	std::vector<DifferentialEquationPreset> presetEquations;
	std::vector<SystemPreset> presetSystems;

	// Удаляет предыдущие фазовые точки (если имеются) и создает новые
	void CreatePhaseTrajectories(function<VectorWrap<float>(double, const VectorWrap<float>&)> speedFunction, size_t dimensionCount, size_t sampleSize, float simulationRadius, float startTime, float endTime, float timeStep, size_t xOrder, size_t yOrder, size_t zOrder, const vector<float>& sliceValues);

	// Удаляет предыдущие фазовые точки
	void ClearPhaseTrajectories();

	// Sets time scale to 1
	void ResetTimeSpeed();

	// Sets time scale to 0
	void Pause();

	function<VectorWrap<float>(double, const VectorWrap<float>&)> GetF(DifferentialEquation* equation);
	function<VectorWrap<float>(double, const VectorWrap<float>&)> GetF(std::vector<Expression>* systemOfODEs);


	void Start();
	void Update();

	void Render(mat4 projectionViewMatrix);

	void OnDestroyRenderer();
};