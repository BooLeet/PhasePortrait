#pragma once
#include "Engine/RendererBehaviour.h"

#include "ExpressionCalculator.h"
#include "Vector.h"
#include <vector>
#include "Engine/Shader.h"

using namespace std;

class DifferentialEquation;
class CoordinateSystemRenderer;
class SphericalCoordinateBehaviour;

class PhasePortrait : public RendererBehaviour
{
	bool startSimulation = false;
	bool stopSimulation = false;

	size_t sampleSize = 1000;
	float simulationSpeed = 1;

	float simulationRadius = 10;
	float simulationStartTimeVolatile = 0;
	float simulationEndTimeVolatile = 10;
	float simulationTimeStepVolatile = 0.1;

	float simulationTimeCounter = 0;

	float simulationStartTime = 0;
	float simulationEndTime = 0;
	float simulationTimeStep = 0;

	float trajectoryMaxLength = 10;
	float trajectoryMinEdgeLength = 0.001;

	enum class TrajectoryColorMode { SingleColor, TwoColor, Rainbow };
	TrajectoryColorMode currentColorMode = TrajectoryColorMode::TwoColor;

	vec3 trajectoryFirstColor = vec3(1, 0, 0);
	vec3 trajectorySecondColor = vec3(0, 0, 1);

	class PhasePointContainer
	{
	public:
		std::vector< VectorWrap<float>> trajectory;

		PhasePointContainer(std::vector< VectorWrap<float>> phaseTrajectory) :  trajectory(phaseTrajectory) {}
	};

	std::vector<std::vector< VectorWrap<float>>> phaseTrajectories;
	//std::vector<std::pair<TrailRenderer*, std::vector<float> >> phasePoints;
	
	size_t xDiffOrder = 0;
	size_t yDiffOrder = 1;
	size_t zDiffOrder = 2;
	std::vector<float> sliceValues;

	std::map<std::string, calcFunction > calculatorFunctions;
	VectorWrap<float> currentPointForCalculation;

	DifferentialEquation* differentialEquation = nullptr;
	std::vector<Expression> systemOfODEs;

	CoordinateSystemRenderer* coordinateRenderer = nullptr;
	SphericalCoordinateBehaviour* cameraHolder = nullptr;

	// Задание дифференциального уравнения
	void SetDifferentialEquation(DifferentialEquation* differentialEquationReference);

	void SetSystemOfODEs(const std::vector<std::vector<char>>& system, char* variables);

	// Вычисляет правую часть дифференциального уравнения для данной фазовой точки
	float CalculateDifferentialEquation(double t, const VectorWrap<float>& point);

	// Вносит необходимые функции для вычисления уравнений
	void CalculatorFunctionSetup();

	// Вносит заранее подготовленные уравнения
	void PresetsSetup();

	// Возвращает значения решения задачи Коши 
	std::vector< VectorWrap<float>> SolveRungeKutta(function<VectorWrap<float>(double, const VectorWrap<float>&)> f, const vector<float>& yStart, double tStart, double tEnd, double tStep);

	// Возвращает новую позицию точки в зависимости от выбранных фазовых осей визуализации
	vec3 GetTrailPosition(const VectorWrap<float>& phasePosition);
	
	// Удаляет предыдущие фазовые точки (если имеются) и создает новые
	void CreatePhaseTrajectories();

	// Удаляет предыдущие фазовые точки
	void ClearPhaseTrajectories();

	vec3 GetTrajectoryColor(float parameter);

public:
	

	void Start();
	void Update();

	void Render(mat4 projectionViewMatrix);

	void OnDestroyRenderer();

	

private:
	enum class UIState
	{
		EquationSetting,SystemSetting,Simulation,StartingMenu
	};

	UIState currentUIState = UIState::StartingMenu;

	// ODE
	char* equationInput = new char[100]();
	char* variablesInput = new char[100]();
	int diffEqOrder = 2;
	std::string equationErrorMessage;

	struct DifferentialEquationPreset
	{
		size_t order;
		std::string equationInput;
		std::string variablesInput;
		std::string equationName;

		DifferentialEquationPreset(size_t equationOrder, const std::string& equation, const std::string& variables, const std::string& name) : order(equationOrder), equationInput(equation), variablesInput(variables), equationName(name) {}
	};
	std::vector<DifferentialEquationPreset> presetEquations;

	// System of ODEs
	int systemSize = 2;
	std::vector<std::vector<char>> systemInput;
	struct SystemPreset
	{
		std::vector<std::vector<char>> systemInput;
		std::string systemName;
		std::string variablesInput;

		SystemPreset(const std::vector<std::string> equationInput, const std::string& variables, const std::string& name) : systemName(name), variablesInput(variables)
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
	std::vector<SystemPreset> presetSystems;

	void HandleUI();

	void EquationSettingUI();

	void SystemSettingUI();

	void SimulationWindowUI();

	void StartingMenuUI();
};