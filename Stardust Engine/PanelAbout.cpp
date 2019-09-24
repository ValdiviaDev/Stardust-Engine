#include "PanelAbout.h"
#include "imgui/imgui.h"

PanelAbout::PanelAbout()
{
	name = "about";
	active = false;
}

PanelAbout::~PanelAbout()
{
}

void PanelAbout::Draw()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	//Set window position and size
	ImGui::SetNextWindowPos({ 70,70 }, ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_Once);

	//Window
	ImGui::Begin("About", &active, window_flags);
	ImGui::Text("Stardust Engine");
	ImGui::Separator();
	ImGui::Text("3D game engine by Ricardo \nGutierrez & David Valdivia.");

	ImGui::End();
}
