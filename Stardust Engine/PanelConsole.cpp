#include "PanelConsole.h"
#include "imgui/imgui.h"

PanelConsole::PanelConsole()
{
	name = "console";
	active = true;
	pos_x = 10, pos_y = 770, width = 700, height = 250;
}

PanelConsole::~PanelConsole()
{
}

void PanelConsole::Draw()
{
	//Set window position and size
	ImGui::SetNextWindowPos(ImVec2(pos_x, pos_y), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Once);
	
	static bool autom_scroll = true;

	//Window
	ImGui::Begin("Console", &active, ImGuiWindowFlags_None);
	
	//Clear button
	ImGui::BeginChild(1, { (float)width, (float)(40) }, true);
	//Automatic scroll
	ImGui::Checkbox("Automatic scroll", &autom_scroll);
	ImGui::SameLine();
	if (ImGui::Button("Clear", { 50, 20 }))
		ConsoleClear();
	
	
	ImGui::EndChild();

	ImGui::BeginChild(2, { (float)width, (float)(160) });

	//Scrolling Logs
	ImGui::TextUnformatted(buff.begin());
	if (scroll && autom_scroll) {
		ImGui::SetScrollHere(1.0f);
		scroll = false;
	}
	ImGui::EndChild();

	ImGui::End();
}

void PanelConsole::AddLog(const char* log)
{
	buff.appendf(log);
	buff.appendf("\n");
	scroll = true;
}

void PanelConsole::ConsoleClear()
{
	buff.clear();
}
