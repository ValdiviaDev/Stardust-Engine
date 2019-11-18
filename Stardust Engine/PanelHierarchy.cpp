#include "PanelHierarchy.h"
#include "GameObject.h"
#include "imgui/imgui.h"
#include "Application.h"

PanelHierarchy::PanelHierarchy()
{
	name = "hierarchy";
	active = true;
	pos_x = 0, pos_y = 99, width = 300, height = 791;
}


PanelHierarchy::~PanelHierarchy()
{
}

void PanelHierarchy::Draw()
{
	if (resize) {
		int x, y;
		App->window->GetWinSize(x, y);
		//height = win height - (panel edit height + panel console height menu bar height)
		ImGui::SetNextWindowPos(ImVec2(pos_x, pos_y), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(width, y - 349), ImGuiCond_Always);
		resize = false;
	}

	ImGui::Begin("Hierarchy", &active, ImGuiWindowFlags_None);
	
	int node_index = 0;
	DrawGOHierarchy(App->scene->GetRootGameObject(), node_index);
	
	ImGui::End();
}

void PanelHierarchy::DrawGOHierarchy(GameObject* go, int & node_index)
{
	if (go != App->scene->GetRootGameObject()) {
		
		bool click = false;
		go->GUIHierarchyPrint(node_index, click);
		node_index++;
	}
	else {
		
		for (uint i = 0; i < go->GetNumChilds(); ++i)
			DrawGOHierarchy(go->GetChild(i), node_index);
	}
}
