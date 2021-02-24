#pragma once
#include "Engine/ObjectBehaviour.h"
#include "TrailRenderer.h"

#include "ExpressionCalculator.h"
#include "Vector.h"
#include <vector>

class DifferentialEquation;
class CoordinateSystemRenderer;

class PhaseFlow : public ObjectBehaviour
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

	GLuint programID;

	class PhasePointContainer
	{
	public:
		TrailRenderer* trail;
		std::vector< std::pair<double, Vector<float>>> trajectory;

		PhasePointContainer(TrailRenderer* trailRenderer, std::vector< std::pair<double, Vector<float>>> phaseTrajectory) : trail(trailRenderer), trajectory(phaseTrajectory) {}
	};


	std::vector<PhasePointContainer> phasePoints;
	//std::vector<std::pair<TrailRenderer*, std::vector<float> >> phasePoints;
	
	size_t xDiffOrder = 0;
	size_t yDiffOrder = 1;
	size_t zDiffOrder = 2;

	std::map<std::string, function > calculatorFunctions;
	Vector<float> currentPointForCalculation;

	DifferentialEquation* differentialEquation = nullptr;

	CoordinateSystemRenderer* coordinateRenderer = nullptr;

	// ��������� ������ ����� ����������������� ��������� ��� ������ ������� �����
	float CalculateDifferentialEquation(double t, const Vector<float>& point);

	// ������ ����������� ������� ��� ���������� ���������
	void CalculatorFunctionSetup();

	// ������ ������� �������������� ���������
	void PresetEquationsSetup();

	// ���������� ������� ���������� �����
	std::vector< std::pair<double, Vector<float>>> GetPhaseTrajectory(const std::vector<float>& startPosition, double startingTime, double endTime, double timeStep);

	// ���������� ������ �������� ��� ������ ������� �����
	Vector<float> GetPhaseSpeedVector(double t, const Vector<float>& point);

	// ���������� ����� ������� ����� � ����������� �� ��������� ������� ���� ������������
	vec3 GetNewTrailPosition(const Vector<float>& phasePosition);
	
	// ������� ���������� ������� ����� (���� �������) � ������� �����
	void PhasePointsSetup();

	// ������� ���������� ������� �����
	void ClearPhasePoints();


	void Simulation();
public:
	size_t sampleSize = 1000;
	
	TrailRenderer::RenderMode renderMode = TrailRenderer::RenderMode::Lines;

	float simulationSpeed = 1;

	void Start();
	void Update();

	void OnDestroy();

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
	int diffEqOrder = 1;
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