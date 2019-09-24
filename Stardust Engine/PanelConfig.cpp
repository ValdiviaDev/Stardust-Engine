#include "PanelConfig.h"
#include "imgui/imgui.h"

PanelConfig::PanelConfig()
{
	name = "config";
	active = true;
}

PanelConfig::~PanelConfig()
{
}

void PanelConfig::Draw()
{
	//Set window position and size
	ImGui::SetNextWindowPos({ 700,70 }, ImGuiWindowFlags_MenuBar);
	ImGui::SetNextWindowSize(ImVec2(400, 900), ImGuiCond_Once);

	//Window
	ImGui::Begin("Configuration", &active, ImGuiWindowFlags_None);
	if (ImGui::BeginMenu("Options"))
	{

		ImGui::EndMenu();
	}
	if (ImGui::CollapsingHeader("Aplication"))
	{
		//if(ImGui::Checkbox("Active", true)){}
	}

	if (ImGui::CollapsingHeader("Window"))
	{
		//if(ImGui::Checkbox("Active", true)){}
	}

	if (ImGui::CollapsingHeader("File System"))
	{
		//if(ImGui::Checkbox("Active", true)){}
	}

	if (ImGui::CollapsingHeader("Input"))
	{
		//if(ImGui::Checkbox("Active", true)){}
	}

	if (ImGui::CollapsingHeader("Hardware"))
	{
		//if(ImGui::Checkbox("Active", true)){}
	}


	ImGui::End();
}
