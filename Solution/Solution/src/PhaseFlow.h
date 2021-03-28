#pragma once
#include "Engine/RendererBehaviour.h"

#include "ExpressionCalculator.h"
#include "Vector.h"
#include <vector>
#include "Engine/Shader.h"

class DifferentialEquation;
class CoordinateSystemRenderer;
class SphericalCoordinateBehaviour;

class PhaseFlow : public RendererBehaviour
{
	bool startSimulation = false;
	bool stopSimulation = false;

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
		std::vector< Vector<float>> trajectory;

		PhasePointContainer(std::vector< Vector<float>> phaseTrajectory) :  trajectory(phaseTrajectory) {}
	};


	std::vector<std::vector< Vector<float>>> phasePoints;
	//std::vector<std::pair<TrailRenderer*, std::vector<float> >> phasePoints;
	
	size_t xDiffOrder = 0;
	size_t yDiffOrder = 1;
	size_t zDiffOrder = 2;

	std::map<std::string, function > calculatorFunctions;
	Vector<float> currentPointForCalculation;

	DifferentialEquation* differentialEquation = nullptr;

	CoordinateSystemRenderer* coordinateRenderer = nullptr;
	SphericalCoordinateBehaviour* cameraHolder = nullptr;

	// ��������� ������ ����� ����������������� ��������� ��� ������ ������� �����
	float CalculateDifferentialEquation(double t, const Vector<float>& point);

	// ������ ����������� ������� ��� ���������� ���������
	void CalculatorFunctionSetup();

	// ������ ������� �������������� ���������
	void PresetEquationsSetup();

	// ���������� ������� ���������� �����
	std::vector< Vector<float>> GetPhaseTrajectory(const std::vector<float>& startPosition, double startingTime, double endTime, double timeStep);

	// ���������� ������ �������� ��� ������ ������� �����
	Vector<float> CalculateDerivativeVectorFunction(double t, const Vector<float>& point);

	// ���������� ����� ������� ����� � ����������� �� ��������� ������� ���� ������������
	vec3 GetTrailPosition(const Vector<float>& phasePosition);
	
	// ������� ���������� ������� ����� (���� �������) � ������� �����
	void PhasePointsSetup();

	// ������� ���������� ������� �����
	void ClearPhasePoints();

	vec3 GetTrajectoryColor(float parameter);

public:
	size_t sampleSize = 1000;

	float simulationSpeed = 1;

	void Start();
	void Update();

	void Render(mat4 projectionViewMatrix);

	void OnDestroyRenderer();

	// ������� ����������������� ���������
	void SetDifferentialEquation(DifferentialEquation* differentialEquationReference);

private:
	enum class UIState
	{
		EquationSetting,Simulation,StartingMenu
	};

	UIState currentUIState = UIState::StartingMenu;
	char* equationInput = new char[100]();
	char* variablesInput = new char[100]();
	int diffEqOrder = 2;
	std::string equationErrorMessage;

	struct DifferentialEquationInput

	{
		size_t order;
		std::string equationInput;
		std::string variablesInput;
		std::string equationName;

		DifferentialEquationInput(size_t equationOrder, const std::string& equation, const std::string& variables, const std::string& name) : order(equationOrder), equationInput(equation), variablesInput(variables), equationName(name) {}
	};
	std::vector<DifferentialEquationInput> presetEquations;

	void HandleUI();

	void EquationSettingUI();

	void SimulationWindowUI();

	void StartingMenuUI();
};