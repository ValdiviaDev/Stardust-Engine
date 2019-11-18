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

	ImGui::Begin("Edit", &active, ImGuiWindowFlags_None);

	// Guizmos TODO

	// Play/Pause
	ImGui::SetCursorPos({ (float)(width / 2 - 30), (float)(height / 3) });
	ImGui::Button("Play", { 50, 35 });
	ImGui::SetCursorPos({ (float)(width / 2 + 30), (float)(height / 3) });
	ImGui::Button("Pause", { 50, 35 });

	// Debug buttons TODO

	ImGui::End();

}
