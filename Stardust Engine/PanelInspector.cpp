#include "PanelInspector.h"
#include "GameObject.h"
#include "imgui/imgui.h"
#include "Application.h"

PanelInspector::PanelInspector()
{
	name = "inspector";
	active = true;
	pos_x = 1261, pos_y = 14, width = 303, height = 794;
}


PanelInspector::~PanelInspector()
{
}

void PanelInspector::Draw()
{
	ImGui::SetNextWindowPos(ImVec2(pos_x, pos_y), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Once);

	ImGui::Begin("Inspector");
	if(App->scene->scene_gameobject)
		App->scene->scene_gameobject->DrawComponentsInspector();
	ImGui::End();
}
