#include "PanelConsole.h"
#include "imgui/imgui.h"

PanelConsole::PanelConsole()
{
	name = "console";
	active = true;
}

PanelConsole::~PanelConsole()
{
}

void PanelConsole::Draw()
{
	//Set window position and size
	ImGui::SetNextWindowPos({ 50,70 }, ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Once);

	//Window
	ImGui::Begin("Console", &active, ImGuiWindowFlags_None);

	ImGui::End();
}
