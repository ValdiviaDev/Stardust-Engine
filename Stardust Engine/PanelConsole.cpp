#include "PanelConsole.h"
#include "imgui/imgui.h"
#include "Application.h"
#include "ModuleWindow.h"

PanelConsole::PanelConsole()
{
	name = "console";
	active = true;
	pos_x = 10, pos_y = 770, width = 540, height = 250;

}

PanelConsole::~PanelConsole()
{
}

void PanelConsole::Draw()
{
	//Set window position and size
	static int x, y;
	static int hierarchy_width = 300;
	static int inspector_width = 360;

	if (resize) {
		App->window->GetWinSize(x, y);

		ImGui::SetNextWindowPos(ImVec2(hierarchy_width, y - height), ImGuiCond_Always);
		width = x - (inspector_width + hierarchy_width);
		ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
		resize = false;
	}


	static bool autom_scroll = true;

	//Window
	ImGui::Begin("Console", &active, ImGuiWindowFlags_None);
	
	//Clear button
	ImGui::BeginChild(1, { (float)width - 20, (float)(40) }, true);
	//Automatic scroll
	ImGui::Checkbox("Automatic scroll", &autom_scroll);
	ImGui::SameLine();
	if (ImGui::Button("Clear", { 50, 20 }))
		ConsoleClear();
	
	
	ImGui::EndChild();

	ImGui::BeginChild(2, { (float)width - 20, (float)(160) });

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

	//if(App->gui->txt_log_init)
	//	ImGui::LogToFile();
	//ImGui::LogText(log);
}

void PanelConsole::ConsoleClear()
{
	buff.clear();
}
