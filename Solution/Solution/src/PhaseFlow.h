#pragma once
#include "Engine/ObjectBehaviour.h"
#include "TrailRenderer.h"

#include "ExpressionCalculator.h"
#include <vector>

//class TrailRenderer;

class PhaseFlow : public ObjectBehaviour
{
	
	float simulationRadius = 6.28;

	
	std::vector<std::pair<TrailRenderer*, std::vector<float> >> phasePoints;
	
	size_t xDiffOrder = 0;
	size_t yDiffOrder = 1;
	size_t zDiffOrder = 2;

	std::map<std::string, function > calculatorFunctions;
	std::vector<float>* currentPointForCalculation;

	float CalculateDifferentialEquation(const std::vector<float>& point);

	void CalculatorFunctionSetup();
	std::vector<float> GetPhaseSpeedVector(std::vector<float>& point);
	vec3 GetNewTrailPosition(const std::vector<float>& phasePosition);
public:
	size_t sampleSize = 22;
	size_t differentialEquationOrder = 2;
	expression rightSideExpression;
	float airResistance = 0.5;
	float g = 10;
	float pendulumLength = 1;
	TrailRenderer::RenderMode renderMode = TrailRenderer::RenderMode::Lines;

	float simulationSpeed = 1;

	void Start();
	void Update();

	void OnDestroy();
};