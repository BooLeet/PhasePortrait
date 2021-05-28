#pragma once
#include <vector>
#include <functional>
#include "Vector.h"
#include "PhasePortrait.h"
//class PhasePortrait;

class PhasePortraitState
{
	PhasePortraitState* nextState = nullptr;
protected :
	void SetNextState(PhasePortraitState* state)
	{
		if (nextState == nullptr)
			nextState = state;
	}
public:
	virtual void Init(PhasePortrait& portrait) = 0;
	virtual void Action(PhasePortrait& portrait) = 0;
	virtual PhasePortraitState* Transition(PhasePortrait& portrait) { return nextState; }
};

namespace PhasePortraitStates
{
	class StartMenuState : public PhasePortraitState
	{
	public:
		void Init(PhasePortrait& portrait);
		void Action(PhasePortrait& portrait);
	};

	class EditEquationState : public PhasePortraitState
	{
		int diffEqOrder = 2;
		char* equationInput;
		char* constantsInput;
		
		size_t xDiffOrder = 0;
		size_t yDiffOrder = 1;
		size_t zDiffOrder = 2;
		std::vector<float>* sliceValues;

	public:
		EditEquationState(size_t order, char* equation, char* constants, size_t xIndx = 0, size_t yIndx = 1, size_t zIndx = 2) : diffEqOrder(order), equationInput(equation), constantsInput(constants), sliceValues(new std::vector<float>(order)), xDiffOrder(xIndx), yDiffOrder(yIndx), zDiffOrder(zIndx) {}
		EditEquationState(size_t order, char* equation, char* constants, std::vector<float>* slice, size_t xIndx = 0, size_t yIndx = 1, size_t zIndx = 2) : diffEqOrder(order), equationInput(equation), constantsInput(constants), sliceValues(slice), xDiffOrder(xIndx), yDiffOrder(yIndx), zDiffOrder(zIndx) {}

		void Init(PhasePortrait& portrait);
		void Action(PhasePortrait& portrait);
	};

	class EditSystemState : public PhasePortraitState
	{
		int systemSize = 3;
		std::vector<std::vector<char>>* systemInput;
		char* constantsInput;

		size_t xVarIndx = 0;
		size_t yVarIndx = 1;
		size_t zVarIndx = 2;
		std::vector<float>* sliceValues;
	public:
		EditSystemState(const std::vector<std::vector<char>>& system, char* constants, size_t xIndx = 0, size_t yIndx = 1, size_t zIndx = 2) : systemInput(new std::vector<std::vector<char>>(system)), constantsInput(constants), sliceValues(new std::vector<float>(system.size())), xVarIndx(xIndx), yVarIndx(yIndx), zVarIndx(zIndx) {}
		EditSystemState(std::vector<std::vector<char>>* system, char* constants, std::vector<float>* slice, size_t xIndx = 0, size_t yIndx = 1, size_t zIndx = 2) : systemInput(system), constantsInput(constants), sliceValues(slice), xVarIndx(xIndx), yVarIndx(yIndx), zVarIndx(zIndx) {}
		EditSystemState(int size, char* constants, size_t xIndx = 0, size_t yIndx = 1, size_t zIndx = 2) : systemInput(new std::vector< std::vector<char>>(size, std::vector<char>(100))), constantsInput(constants), sliceValues(new std::vector<float>(size)), xVarIndx(xIndx), yVarIndx(yIndx), zVarIndx(zIndx) {}

		void Init(PhasePortrait& portrait);
		void Action(PhasePortrait& portrait);
	};

	class PlaybackState : public PhasePortraitState
	{
		size_t sampleSize = 1000;
		float simulationRadius = 10;

		float startTime = 0;
		float endTime = 10;
		float timeStep = 0.1;

		size_t xVarIndx = 0;
		size_t yVarIndx = 1;
		size_t zVarIndx = 2;

		std::function<VectorWrap<float>(double, const VectorWrap<float>&)> F;

		std::vector<float>* sliceValues;
		bool equationMode;
		// Equation data
		size_t diffEqOrder;
		char* equationInput;
		char* constantsInput;
		DifferentialEquation* differentialEquation;
		// System data
		std::vector<Expression> systemOfODEs;
		std::vector<std::vector<char>>* systemInput;

	public:
		
		PlaybackState(size_t diffOrder, char* equation, char* constants, std::vector<float>* slice, PhasePortrait& portrait, size_t xIndx = 0, size_t yIndx = 1, size_t zIndx = 2);
		PlaybackState(std::vector<std::vector<char>>* system, char* constants, std::vector<float>* slice, PhasePortrait& portrait, size_t xIndx = 0, size_t yIndx = 1, size_t zIndx = 2);

		void Init(PhasePortrait& portrait);
		void Action(PhasePortrait& portrait);

		~PlaybackState()
		{
			delete differentialEquation;
		}
	};
}
