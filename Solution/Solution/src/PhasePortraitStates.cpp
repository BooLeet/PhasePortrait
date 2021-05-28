#include "PhasePortraitStates.h"
#include "Engine/vendor/imgui/imgui.h"
#include "Engine/vendor/imgui/imgui_impl_glfw_gl3.h"


#include "Engine/Engine.h"
#include "Engine/Scene.h"
#include "Engine/SceneObject.h"

#include "Engine/OpenGLComponents.h"
#include "PhasePortrait.h"
#include "Engine/Utility.h"
#include "CoordinateSystemRenderer.h"
#include "DifferentialEquation.h"
#include "Engine/SphericalCoordinateBehaviour.h"

#include <string>
#include <fstream>
#include <sstream>
#include <strstream>

using namespace PhasePortraitStates;

void StartMenuState::Init(PhasePortrait& portrait)
{

}

void StartMenuState::Action(PhasePortrait& portrait)
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowContentWidth(portrait.GetEngine()->GetWindowWidth() / 3);
	ImGui::Begin("Starting Menu");

	if (ImGui::Button("Custom Equation"))
		SetNextState(new EditEquationState(1, new char[100](), new char[100]()));

	for (auto& preset : portrait.presetEquations)
	{
		if (ImGui::Button(preset.equationName.c_str()))
		{
			char* equationInput = new char[100]();
			char* constantsInput = new char[100]();

			for (size_t i = 0; i < preset.equationInput.size(); ++i)
				equationInput[i] = preset.equationInput[i];

			for (size_t i = 0; i < preset.constantsInput.size(); ++i)
				constantsInput[i] = preset.constantsInput[i];

			SetNextState(new EditEquationState(preset.order, equationInput, constantsInput));
		}
	}

	ImGui::Separator();

	if (ImGui::Button("Custom System of ODEs"))
		SetNextState(new EditSystemState(3, new char[100]()));

	for (auto& preset : portrait.presetSystems)
	{
		if (ImGui::Button(preset.systemName.c_str()))
		{
			char* constantsInput = new char[100]();
			for (size_t i = 0; i < preset.constantsInput.size(); ++i)
				constantsInput[i] = preset.constantsInput[i];

			SetNextState(new EditSystemState(preset.systemInput, constantsInput));
		}
	}

	ImGui::Spacing();
	if (ImGui::Button("Quit"))
		portrait.GetEngine()->Quit();

	ImGui::End();
}

void EditEquationState::Init(PhasePortrait& portrait)
{
}

void EditEquationState::Action(PhasePortrait& portrait)
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowContentWidth(portrait.GetEngine()->GetWindowWidth() / 3);
	ImGui::Begin("Equation Editing");

	if (ImGui::InputInt("Equation Order", &diffEqOrder))
	{
		if (diffEqOrder < 1)
			diffEqOrder = 1;
		delete sliceValues;
		sliceValues = new std::vector<float>(diffEqOrder);
	}

	ImGui::Text("diff(%d) = ", diffEqOrder); ImGui::SameLine();

	ImGui::InputText("", equationInput, 100);
	ImGui::InputTextMultiline("Constants", constantsInput, 100);

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
				ImGui::InputFloat((to_string(i) + " order derivative value").c_str(), &((*sliceValues)[i]));
		}
	}

	if (ImGui::Button("Confirm Equation"))
	{
		// validation

		// Setting up coordinate axis
		bool coordY = diffEqOrder >= 2;
		bool coordZ = diffEqOrder >= 3;
		portrait.axisRenderer->SetAxis(true, coordY, coordZ);

		SetNextState(new PlaybackState(diffEqOrder, equationInput, constantsInput, sliceValues, portrait, xDiffOrder, yDiffOrder, zDiffOrder));
	}

	if (ImGui::Button("Starting Menu"))
	{
		// CLEANUP!
		delete sliceValues;
		delete[] equationInput;
		delete[] constantsInput;
		SetNextState(new StartMenuState());
	}


	ImGui::End();
}

void EditSystemState::Init(PhasePortrait& portrait)
{
}

void EditSystemState::Action(PhasePortrait& portrait)
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowContentWidth(portrait.GetEngine()->GetWindowWidth() / 3);
	ImGui::Begin("System Editing");
	
	if (ImGui::InputInt("System size", &systemSize))
	{
		if (systemSize < 1)
			systemSize = 1;
		sliceValues =  new std::vector<float>(systemSize);
		systemInput = new std::vector<std::vector<char>>(systemSize, vector<char>(100));
	}
	
	for (int i = 0; i < systemSize; ++i)
	{
		ImGui::Text("d(var(%d))/dt = ", i); ImGui::SameLine();
		ImGui::InputText(std::to_string(i).c_str(), &((*systemInput)[i][0]), 100);
	}
	
	ImGui::InputTextMultiline("Constants", constantsInput, 100);
	
	ImGui::Spacing();
	
	if (systemSize > 1)
	{
		int xOrder = xVarIndx;
		ImGui::InputInt("x var index", &xOrder);
		xVarIndx = Clamp<int>(xOrder, 0, systemSize - 1);
	
		int yOrder = yVarIndx;
		ImGui::InputInt("y var index", &yOrder);
		yVarIndx = Clamp<int>(yOrder, 0, systemSize - 1);
	}
	
	if (systemSize > 2)
	{
		int zOrder = zVarIndx;
		ImGui::InputInt("z var index", &zOrder);
		zVarIndx = Clamp<int>(zOrder, 0, systemSize - 1);
	}
	
	if (systemSize > 3)
	{
		for (size_t i = 0; i < systemSize; ++i)
			if (i != xVarIndx && i != yVarIndx && i != zVarIndx)
				ImGui::InputFloat(("var(" + to_string(i) + ") value").c_str(), &((*sliceValues)[i]));
	}
	
	if (ImGui::Button("Confirm System"))
	{
		// Setting up coordinate axis
		bool coordY = systemSize >= 2;
		bool coordZ = systemSize >= 3;
		portrait.axisRenderer->SetAxis(true, coordY, coordZ);
		SetNextState(new PlaybackState(systemInput, constantsInput, sliceValues, portrait, xVarIndx, yVarIndx, zVarIndx));
	}
	
	if (ImGui::Button("Starting Menu"))
	{
		// CLEANUP!
		delete sliceValues;
		delete systemInput;
		delete[] constantsInput;
		SetNextState(new StartMenuState());
	}
	
	ImGui::End();
}

PhasePortraitStates::PlaybackState::PlaybackState(size_t diffOrder, char* equation, char* constants, std::vector<float>* slice, PhasePortrait& portrait, size_t xIndx, size_t yIndx, size_t zIndx)
{
	xVarIndx = xIndx;
	yVarIndx = yIndx;
	zVarIndx = zVarIndx;

	diffEqOrder = diffOrder;
	equationInput = equation;
	constantsInput = constants;
	sliceValues = slice;
	equationMode = true;
	std::stringstream tempStream;
	tempStream << equationInput << '\n' << constantsInput;
	differentialEquation = new DifferentialEquation(diffEqOrder, Expression::ReadExpression(tempStream));
	F = portrait.GetF(differentialEquation);
}

PhasePortraitStates::PlaybackState::PlaybackState(std::vector<std::vector<char>>* system, char* constants, std::vector<float>* slice, PhasePortrait& portrait, size_t xIndx, size_t yIndx, size_t zIndx)
{
	xVarIndx = xIndx;
	yVarIndx = yIndx;
	zVarIndx = zIndx;

	systemInput = system;
	constantsInput = constants;
	sliceValues = slice;
	equationMode = false;

	systemOfODEs.reserve(system->size());
	
	for (size_t i = 0; i < system->size(); ++i)
	{
		std::stringstream tempStream;
		for (size_t j = 0; j < (*system)[i].size(); ++j)
			if((*system)[i][j] != 0)
				tempStream << (*system)[i][j];
		tempStream << '\n' << constantsInput;
		systemOfODEs.push_back(Expression::ReadExpression(tempStream));
	}
	F = portrait.GetF(&systemOfODEs);
}

void PhasePortraitStates::PlaybackState::Init(PhasePortrait& portrait)
{
	
}

void PhasePortraitStates::PlaybackState::Action(PhasePortrait& portrait)
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowContentWidth(portrait.GetEngine()->GetWindowWidth() / 3);
	ImGui::Begin("Playback Controls", nullptr, ImGuiWindowFlags_NoSavedSettings);

	ImGui::Text("%.1f FPS (%.3f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);

	if (equationMode)
	{
		if (ImGui::Button("Edit Equation"))
		{
			portrait.ClearPhaseTrajectories();
			SetNextState(new EditEquationState(diffEqOrder, equationInput, constantsInput,sliceValues, xVarIndx, yVarIndx, zVarIndx));
		}
	}
	else
	{
		if (ImGui::Button("Edit System"))
		{
			portrait.ClearPhaseTrajectories();
			SetNextState(new EditSystemState(systemInput, constantsInput, sliceValues, xVarIndx, yVarIndx, zVarIndx));
		}
	}

	if (ImGui::Button("Starting Menu"))
	{
		portrait.ClearPhaseTrajectories();
		// CLEANUP!
		delete[] equationInput;
		delete[] constantsInput;

		SetNextState(new StartMenuState());
	}

	ImGui::Spacing();

	// PLAYER
	if (ImGui::Button("Create portrait"))
	{
		size_t dimensionCount = equationMode ? diffEqOrder : systemOfODEs.size();
		portrait.CreatePhaseTrajectories(F, dimensionCount, sampleSize, simulationRadius, startTime, endTime, timeStep, xVarIndx, yVarIndx, zVarIndx, *sliceValues);
	}

	if (ImGui::Button("Clear portrait"))
		portrait.ClearPhaseTrajectories();

	ImGui::Spacing();

	if (ImGui::Button("Reset time speed"))
		portrait.ResetTimeSpeed();

	if (ImGui::Button("Pause"))
		portrait.Pause();

	ImGui::Spacing();

	float timeScale = portrait.GetEngine()->timeScale;
	ImGui::SliderFloat("Simulation speed", &timeScale, 0.0f, 10.0f);
	portrait.GetEngine()->timeScale = timeScale;

	ImGui::SliderFloat("Simulation time", &portrait.simulationTimeCounter, portrait.simulationStartTime, portrait.simulationEndTime);

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
		ImGui::InputFloat("Start time", &startTime);
		ImGui::InputFloat("End time", &endTime);
		if (endTime < startTime)
			endTime = startTime;

		ImGui::InputFloat("Simulation time step", &timeStep);
		if (timeStep <= 0)
			timeStep = 0.001;

		ImGui::Separator();
	}

	// TRAJECTORY
	ImGui::Spacing();
	if (ImGui::CollapsingHeader("Trajectory settings"))
	{
		ImGui::InputFloat("Max length", &portrait.trajectorySettings.maxLength);
		if (portrait.trajectorySettings.maxLength <= 0)
			portrait.trajectorySettings.maxLength = 0.01;

		ImGui::InputFloat("Min edge length", &portrait.trajectorySettings.minEdgeLength);
		if (portrait.trajectorySettings.minEdgeLength <= 0)
			portrait.trajectorySettings.minEdgeLength = 0;

		if (ImGui::CollapsingHeader("Background color"))
			ImGui::ColorPicker3("Background color", &portrait.GetEngine()->clearColor[0]);

		const char* colorModes[] = { "Single Color", "Two Color", "Rainbow" };

		if (ImGui::Button("Select color mode"))
			ImGui::OpenPopup("select");
		ImGui::SameLine();
		ImGui::TextUnformatted(colorModes[(int)portrait.trajectorySettings.currentColorMode]);
		if (ImGui::BeginPopup("select"))
		{
			for (int i = 0; i < IM_ARRAYSIZE(colorModes); i++)
				if (ImGui::Selectable(colorModes[i]))
					portrait.trajectorySettings.currentColorMode = (PhasePortrait::TrajectorySettings::ColorMode)i;
			ImGui::EndPopup();
		}

		switch (portrait.trajectorySettings.currentColorMode)
		{
		case PhasePortrait::TrajectorySettings::ColorMode::SingleColor:
			ImGui::ColorPicker3("Color", &portrait.trajectorySettings.firstColor[0]);
			break;
		case PhasePortrait::TrajectorySettings::ColorMode::TwoColor:
			ImGui::ColorPicker3("Head color", &portrait.trajectorySettings.firstColor[0]);
			ImGui::ColorPicker3("Tail color", &portrait.trajectorySettings.secondColor[0]);
			break;
		default:
			break;
		}

		ImGui::Separator();
	}

	// COORDINATES
	ImGui::Spacing();
	ImGui::Checkbox("Show coordinates", &(portrait.axisRenderer->enabled));

	static float coordinateRadius = 20;
	static float segmentLength = 1;
	static float segmentNotchSize = 0.1;

	if (portrait.axisRenderer->enabled)
		if (ImGui::CollapsingHeader("Coordinates settings"))
		{
			if (ImGui::InputFloat("Radius", &coordinateRadius))
				portrait.axisRenderer->SetRadius(coordinateRadius);


			if (ImGui::InputFloat("Segment length", &segmentLength))
				portrait.axisRenderer->SetSegmentLength(segmentLength);


			if (ImGui::InputFloat("Segment notch size", &segmentNotchSize))
				portrait.axisRenderer->SetSegmentNotchSize(segmentNotchSize);

			ImGui::Spacing();

			vec3 xColor = portrait.axisRenderer->GetColorX();
			vec3 yColor = portrait.axisRenderer->GetColorY();
			vec3 zColor = portrait.axisRenderer->GetColorZ();

			if (ImGui::ColorPicker3("Color X", &xColor[0]))
				portrait.axisRenderer->SetColorX(xColor);

			if (ImGui::ColorPicker3("Color Y", &yColor[0]))
				portrait.axisRenderer->SetColorY(yColor);

			if (ImGui::ColorPicker3("Color Z", &zColor[0]))
				portrait.axisRenderer->SetColorZ(zColor);

			ImGui::Separator();
		}

	// CAMERA HOLDER INPUT
	ImGui::Spacing();
	if (ImGui::CollapsingHeader("Camera settings"))
	{
		if (ImGui::Button("Reset camera"))
			portrait.cameraHolder->Reset(10);

		ImGui::InputFloat3("Pivot", &(portrait.cameraHolder->pivot[0]));
		vec3 rotation = (180 / pi<float>()) * vec3(portrait.cameraHolder->angleU, portrait.cameraHolder->angleV, portrait.cameraHolder->angleW);
		if (ImGui::InputFloat3("Rotation", &(rotation[0])))
		{
			rotation = (pi<float>() / 180) * rotation;
			portrait.cameraHolder->angleU = rotation.x;
			portrait.cameraHolder->angleV = rotation.y;
			portrait.cameraHolder->angleW = rotation.z;
		}

		ImGui::Separator();
	}
	portrait.cameraHolder->HandleInput();

	ImGui::End();
}