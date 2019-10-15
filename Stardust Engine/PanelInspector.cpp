#include "PanelInspector.h"
#include "GameObject.h"
#include "imgui/imgui.h"
#include "Application.h"

PanelInspector::PanelInspector()
{
	name = "inspector";
	active = true;
	pos_x = 1261, pos_y = 20, width = 303, height = 794;
}


PanelInspector::~PanelInspector()
{
}

void PanelInspector::Draw()
{
	if (resize) {
		int x, y;
		App->window->GetWinSize(x, y);
		ImGui::SetNextWindowPos(ImVec2(x - width, pos_y), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(width, y), ImGuiCond_Always);
		resize = false;
	}

	ImGui::Begin("Inspector");
	if(App->scene->scene_gameobject)
		App->scene->scene_gameobject->DrawComponentsInspector();
	ImGui::End();
}
