#include "PanelConfig.h"
#include "imgui/imgui.h"
#include "Application.h"

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
		FillFPSVector();
		char title[25];
		sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
		ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
		//sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
		//ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));

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

void PanelConfig::FillFPSVector()
{
	if (fps_log.size() < 30)
		fps_log.push_back(App->GetFPS());
	else
	{
		for (int i = 0; i < 30 - 1; i++)
			fps_log[i] = fps_log[i + 1];

		fps_log[29] = App->GetFPS();
	}
}
