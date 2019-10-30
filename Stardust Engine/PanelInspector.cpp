#include "PanelInspector.h"
#include "GameObject.h"
#include "imgui/imgui.h"
#include "Application.h"
#include "ModuleScene.h"

PanelInspector::PanelInspector()
{
	name = "inspector";
	active = true;
	pos_x = 1261, pos_y = 20, width = 360, height = 794;
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

	GameObject* focused = GetFocusedGameObject(App->scene->GetRootGameObject());
	if(focused)
		focused->DrawComponentsInspector();
	ImGui::End();
}


GameObject* PanelInspector::GetFocusedGameObject(GameObject* root) {

	GameObject* ret = nullptr;

	for (std::vector<GameObject*>::const_iterator it = root->childs.begin(); it < root->childs.end(); it++) {

		if ((*it)->focused)
			return (*it);
		else
			ret = GetFocusedGameObject((*it));

		if (ret) {
			if (ret->focused)
				break;
		}
	}

	return ret;
}