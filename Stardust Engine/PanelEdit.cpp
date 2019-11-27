#include "PanelEdit.h"
#include "Application.h"
#include "imgui/imgui.h"
#include "ModuleTimeManager.h"
#include "ModuleWindow.h"

PanelEdit::PanelEdit()
{
	name = "edit";
	active = true;
	pos_x = 0, pos_y = 19, width = 266, height = 80;

}


PanelEdit::~PanelEdit()
{
}

void PanelEdit::Draw()
{
	if (resize) {
		int x, y;
		App->window->GetWinSize(x, y);
		width = x;

		ImGui::SetNextWindowPos(ImVec2(pos_x, pos_y), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
		resize = false;
	}

	static const char* play_but = "Play";
	static const char* pause_but = "Pause";
	static uint time_scale_width = 100;
	EngineState engine_state = App->GetEngineState();

	ImGui::Begin("Edit", &active, ImGuiWindowFlags_None);

	// Guizmos TODO

	// Play/Pause
	ImGui::SetCursorPos({ (float)(width / 2 - 35), (float)(height / 3) });
	
	//Play/Stop
	if (ImGui::Button(play_but, { 55, 35 })) {
		if (engine_state != Engine_State_Editor) {
			App->Stop();
			play_but = "Play";
			pause_but = "Pause";
		}
		else {
			if(App->Play())
				play_but = "Stop";
		}
	}
	
	//Pause
	ImGui::SetCursorPos({ (float)(width / 2 + 35), (float)(height / 3) });
	if (ImGui::Button(pause_but, { 55, 35 })) {
		if (engine_state == Engine_State_Play)
			pause_but = "Unpause";
		else if (engine_state == Engine_State_Pause)
			pause_but = "Pause";
		
		App->Pause();
	}


	//Time scale slider
	ImGui::PushItemWidth(time_scale_width);
	ImGui::SetCursorPos({ (float)(width / 2 + time_scale_width), (float)(height / 2.5f) });
	ImGui::SliderFloat("Time scale", &App->time->time_scale, 0.0f, 2.0f, "%.1f");
	

	// Debug buttons TODO

	ImGui::End();

}
