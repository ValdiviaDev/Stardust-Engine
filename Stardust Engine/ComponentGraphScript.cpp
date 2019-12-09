#include "Application.h"
#include "ComponentGraphScript.h"
#include "ModuleResourceManager.h"
#include "ResourceGraphScript.h"
#include "NodeGraph.h"
#include "imgui/imgui.h"


ComponentGraphScript::ComponentGraphScript(GameObject* parent) : Component(parent)
{
	type = Comp_Graph_Script;
}


ComponentGraphScript::~ComponentGraphScript()
{
	//Unload Resource
	if (uuid_script != 0) {
		ResourceGraphScript* res = (ResourceGraphScript*)App->resources->Get(uuid_script);
		if (res)
			res->UnloadToMemory();
	}

}

void ComponentGraphScript::Update(float dt)
{
	if (has_script){
		ResourceGraphScript* res = (ResourceGraphScript*)App->resources->Get(uuid_script);
		res->node_graph->Update(dt, gameObject);

		if(show_graph)
			res->node_graph->Draw();
	}
		
}

void ComponentGraphScript::DrawInspector()
{
	//TODO: Close individual graph scripts
	if (ImGui::CollapsingHeader("Graph Script", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Checkbox("Script Active", &active);

		if (!has_script) {
			ImGui::Button("Drag script here");
			if (ImGui::Button("New Script", { 80,30 })) {
				uuid_script = App->GenerateUUID();
				//TODO: Script serialization
				//TODO: Do this in resource manager
				ResourceGraphScript* res = (ResourceGraphScript*)App->resources->CreateNewResource(Resource_Graph_Script, uuid_script);
				res->LoadToMemory();

				has_script = true;
			}
		}
		else
			if (ImGui::Button("Show Graph", { 80,30 }))
				show_graph = !show_graph;

	}

}
