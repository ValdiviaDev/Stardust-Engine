#include "Application.h"
#include "ComponentGraphScript.h"
#include "ModuleResourceManager.h"
#include "ResourceGraphScript.h"
#include "NodeGraph.h"
#include "GameObject.h"
#include "imgui/imgui.h"


ComponentGraphScript::ComponentGraphScript(GameObject* parent) : Component(parent)
{
	type = Comp_Graph_Script;
	BB_objects.push_back(gameObject);
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

		if(show_graph && gameObject->focused)
			res->node_graph->Draw(BB_objects);

		if(App->GetEngineState() != Engine_State_Editor)
			res->node_graph->Update(dt, BB_objects);
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
		else {
			const char* graph_visib_str;
			if (!show_graph)
				graph_visib_str = "Show Graph";
			else
				graph_visib_str = "Hide Graph";

			if (ImGui::Button(graph_visib_str, { 80,30 }))
				show_graph = !show_graph;
			
			ImGui::Separator();

			//Blackboard
			ImGui::Text("Blackboard (GameObjects)");
			ImGui::Separator();
			ImGui::Button("Drag GameObject to add to Blackboard");
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Hierarchy")) {
					GameObject* go = *(GameObject**)payload->Data;
					if (go)
						BB_objects.push_back(go);

				}
				ImGui::EndDragDropTarget();
			}

			//Print objects in blackboard
			for (int i = 1; i < BB_objects.size(); ++i) {
				ImGui::Text(BB_objects[i]->GetName());
			}
		}

	}

}
