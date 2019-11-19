#include "PanelEdit.h"
#include "Application.h"
#include "imgui/imgui.h"


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
	ImGui::Begin("Edit", &active, ImGuiWindowFlags_None);

	// Guizmos TODO

	// Play/Pause
	ImGui::SetCursorPos({ (float)(width / 2 - 35), (float)(height / 3) });
	

	if (ImGui::Button(play_but, { 50, 35 })) {
		ChangeCameraView();

		if (playing)
			play_but = "Stop";
		else
			play_but = "Play";
	}
	
	ImGui::SetCursorPos({ (float)(width / 2 + 35), (float)(height / 3) });
	ImGui::Button("Pause", { 50, 35 });

	// Debug buttons TODO

	ImGui::End();

}

void PanelEdit::ChangeCameraView()
{
	if (!playing) {
		if (App->scene->GetMainCamera() != nullptr) {
			App->camera->current_cam = App->scene->GetMainCamera();
			App->renderer3D->RecalculateProjMat();
			
			//Save scene tmp
			std::string aux = LIBRARY_FOLDER;
			aux.append("tmp_scene.json");
			scene.SaveScene(aux.c_str());
			
			playing = true;
		}
		else
			App->gui->AddLogToConsole("ERROR: You don't have a Main Camera GameObject on the scene!");
	}
	else {
		App->camera->current_cam = App->camera->engine_cam;
		App->renderer3D->RecalculateProjMat();

		//Load scene tmp
		std::string aux = LIBRARY_FOLDER;
		aux.append("tmp_scene.json");
		scene.LoadScene(aux.c_str());

		playing = false;
	}
}
