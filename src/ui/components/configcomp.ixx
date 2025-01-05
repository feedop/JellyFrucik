export module gui:configcomp;

import std;
import <imgui.h>;

import :controller;
import config;
import imguiext;
import simulation;
import math;

export void GuiController::renderConfig(const SimulationData& data)
{
	static bool configChanged = false;
	static bool inertiaChanged = false;

	ImGui::Text("Display settings:");
	ImGui::Checkbox("Show grid", &renderer.drawGrid);
	ImGui::Checkbox("Show cube", &renderer.drawCube);
	ImGui::Checkbox("Show wireframe", &renderer.drawWireframe);
	ImGui::Checkbox("Show bounding box", &renderer.drawBox);
	ImGui::NewLine();

	ImGui::Text("Simulation settings:");
	configChanged |= ImGui::SliderFloat("dt", &config.dt, 0.001f, 1.0f);

	if (ImGui::Button("Restart"))
	{
		simulation.setData(data);
	}	

	if (simulation.isPaused())
	{
		if (ImGui::Button("Resume"))
			simulation.resume();
	}
	else
	{
		if (ImGui::Button("Pause"))
			simulation.pause();

		if (ImGui::Button("Disturb"))
			simulation.disturb();
	}

	ImGui::NewLine();
	configChanged |= ImGui::SliderFloat("Disturbance factor", &config.disturbance, 0.0f, 2.0f);
	configChanged |= ImGui::Checkbox("Disturb velocities instead of positions", &config.disturbVelocities);
	configChanged |= ImGui::SliderFloat("Mass", &config.m, 0.001f, 100.0f);
	configChanged |= ImGui::SliderFloat("Internal spring coefficient (c1)", &config.c1, 0.01f, 100.0f);
	configChanged |= ImGui::SliderFloat("External spring coefficient (c2)", &config.c2, 0.01f, 100.0f);
	configChanged |= ImGui::SliderFloat("Damping coefficient (k)", &config.k, 0.0f, 10.0f);
	configChanged |= ImGui::SliderFloat("Collision elasticity (mi)", &config.mi, 0.0f, 1.0f);
	
	if (configChanged)
		simulation.setConfig(config);
}