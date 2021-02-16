#pragma once
#include "Engine/ObjectBehaviour.h"
#include "TrailRenderer.h"

#include "ExpressionCalculator.h"
#include <vector>

class DifferentialEquation;
class CoordinateSystemRenderer;

class PhaseFlow : public ObjectBehaviour
{
	bool startSimulation = false;
	bool stopSimulation = false;

	float simulationRadius = 6.28;

	GLuint programID;
	std::vector<std::pair<TrailRenderer*, std::vector<float> >> phasePoints;
	
	size_t xDiffOrder = 0;
	size_t yDiffOrder = 1;
	size_t zDiffOrder = 2;

	std::map<std::string, function > calculatorFunctions;
	std::vector<float>* currentPointForCalculation;

	DifferentialEquation* differentialEquation = nullptr;

	CoordinateSystemRenderer* coordinateRenderer = nullptr;

	// Вычисляет правую часть дифференциального уравнения для данной фазовой точки
	float CalculateDifferentialEquation(std::vector<float>& point);

	// Вносит необходимые функции для вычисления уравнений
	void CalculatorFunctionSetup();

	// Возвращает вектор скорости для данной фазовой точки
	std::vector<float> GetPhaseSpeedVector(std::vector<float>& point);

	// Возвращает новую позицию точки в зависимости от выбранных фазовых осей визуализации
	vec3 GetNewTrailPosition(const std::vector<float>& phasePosition);
	
	// Удаляет предыдущие фазовые точки (если имеются) и создает новые
	void PhasePointsSetup();

	// Удаляет предыдущие фазовые точки
	void ClearPhasePoints();
public:
	size_t sampleSize = 22;
	
	TrailRenderer::RenderMode renderMode = TrailRenderer::RenderMode::Lines;

	float simulationSpeed = 1;

	void Start();
	void Update();

	void OnDestroy();

	// Задание дифференциального уравнения
	void SetDifferentialEquation(DifferentialEquation* differentialEquationReference);

private:
	enum class UIState
	{
		EquationSetting,Simulation,EditEquation
	};

	UIState currentUIState = UIState::EquationSetting;
	char* equationInput = new char[100]();
	char* variablesInput = new char[100]();

	void HandleUI();

	void EquationSettingUI();

	void SimulationWindowUI();

	void EditEquationUI();
};