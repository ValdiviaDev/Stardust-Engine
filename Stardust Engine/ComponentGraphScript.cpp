#include "Application.h"
#include "ComponentGraphScript.h"
#include "ModuleResourceManager.h"
#include "ResourceGraphScript.h"
#include "NodeGraph.h"
#include "GameObject.h"
#include "ModuleGui.h"
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
	if (HasScript()){
		ResourceGraphScript* res = (ResourceGraphScript*)App->resources->Get(uuid_script);

		if(show_graph && gameObject->focused)
			res->node_graph->Draw(BB_objects, active);

		if(App->GetEngineState() != Engine_State_Editor && active)
			res->node_graph->Update(dt, BB_objects);
	}
		
}

void ComponentGraphScript::DrawInspector()
{
	//TODO: Close individual graph scripts
	if (ImGui::CollapsingHeader("Graph Script", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Checkbox("Script Active", &active);

		if (!HasScript()) {
			ImGui::Button("Drag script here");
			if (ImGui::Button("New Script", { 80,30 })) {
				uuid_script = App->GenerateUUID();
				//TODO: Script serialization
				//TODO: Do this in resource manager
				ResourceGraphScript* res = (ResourceGraphScript*)App->resources->CreateNewResource(Resource_Graph_Script, uuid_script);
				res->LoadToMemory();

			}
		}
		//If a graph node is already associated with this component
		else {
			const char* graph_visib_str;
			if (!show_graph)
				graph_visib_str = "Show Graph";
			else
				graph_visib_str = "Hide Graph";

			if (ImGui::Button(graph_visib_str, { 80,30 })) {
				if (!show_graph) 
					HideOtherGraphsFromSameObject();
				
				show_graph = !show_graph;
			}
			
			ImGui::Separator();

			//Blackboard
			ImGui::Text("Blackboard (GameObjects)");
			ImGui::Separator();
			ImGui::Button("Drag GameObject to add to Blackboard");
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Hierarchy")) {
					GameObject* go = *(GameObject**)payload->Data;
					AddReferenceToBlackboard(go);
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

bool ComponentGraphScript::HasScript()
{
	if (uuid_script != 0)
		return true;

	return false;
}

void ComponentGraphScript::AddReferenceToBlackboard(GameObject* ref)
{
	if (ref) {
		bool is_ref_added = false;

		for (std::vector<GameObject*>::const_iterator it = BB_objects.begin(); it < BB_objects.end(); it++) {
			if ((*it) == ref) {
				is_ref_added = true;
				break;
			}
		}
		if (!is_ref_added)
			BB_objects.push_back(ref);
		else
			App->gui->AddLogToConsole("This reference is already added to this blackboard");

	}
}

bool ComponentGraphScript::DeleteGameObjectFromBlackboard(GameObject* to_delete)
{

	//Search for every reference, if it exists, delete it off the list
	for (std::vector<GameObject*>::const_iterator it = BB_objects.begin(); it < BB_objects.end(); it++) {
		if ((*it) == to_delete) {
			BB_objects.erase(it);
			BB_objects.shrink_to_fit();
			return true;
		}	
	}


	return false;
}

void ComponentGraphScript::HideOtherGraphsFromSameObject()
{
	for (int i = 0; i < gameObject->GetNumComp(); ++i) {
		if (gameObject->GetComponentByIndex(i)->GetType() == Comp_Graph_Script) {
			ComponentGraphScript* g = (ComponentGraphScript*)gameObject->GetComponentByIndex(i);
			g->show_graph = false;
		}
	}
}
