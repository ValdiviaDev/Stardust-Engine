#include "Application.h"
#include "ComponentGraphScript.h"
#include "imgui/imgui.h"


ComponentGraphScript::ComponentGraphScript(GameObject* parent) : Component(parent)
{
	type = Comp_Graph_Script;
}


ComponentGraphScript::~ComponentGraphScript()
{
}

void ComponentGraphScript::DrawInspector()
{
	//TODO: Close individual graph scripts
	if (ImGui::CollapsingHeader("Graph Script", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (has_script) {
			ImGui::Button("Drag script here");
			ImGui::Button("New Script", { 80,30 });
		}
	}

}
