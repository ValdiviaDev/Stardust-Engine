#include "Application.h"
#include "ComponentGraphScript.h"
#include "ModuleResourceManager.h"
#include "ResourceGraphScript.h"
#include "NodeGraph.h"
#include "GameObject.h"
#include "ModuleGui.h"
#include "ModuleScene.h"
#include "imgui/imgui.h"
#include "PanelAssets.h"


ComponentGraphScript::ComponentGraphScript(GameObject* parent, uint script_num) : Component(parent)
{
	type = Comp_Graph_Script;
	BB_objects.push_back(gameObject);
	
	this->script_num = script_num;
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
			res->node_graph->Draw(BB_objects, active, show_graph);

		if(App->GetEngineState() != Engine_State_Editor && active && gameObject->IsActive())
			res->node_graph->Update(dt, BB_objects, script_num);
	}

	if (new_script_clicked)
		NewScriptMenu();
		
}

void ComponentGraphScript::DrawInspector()
{
	std::string cmp_idx = "";
	for (int i = 0; i < gameObject->GetNumComp(); ++i) {
		if (gameObject->GetComponentByIndex(i) == this)
			cmp_idx = "##" + std::to_string(i);
	}

	std::string c_name = "Graph Script" + cmp_idx;
	if (ImGui::CollapsingHeader(c_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
		
		std::string save_scr = "Save Script:" + cmp_idx;
		if (ImGui::Button(save_scr.c_str(), ImVec2(100, 0))) {

			if (HasScript())
				SaveScriptFile(uuid_script);
		}

		ImGui::SameLine();
		ImGui::Text(script_name);
	
		

		std::string scr_active = "Script Active" + cmp_idx;
		ImGui::Checkbox(scr_active.c_str(), &active);

		if (!HasScript()) {
			//Preexisting script
			ImGui::Button("Drag script here");

			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets_Script")) {
					UID uuid = *(UID*)payload->Data;
					if (App->resources->Get(uuid) != nullptr) {
						uuid_script = uuid;
						ResourceGraphScript* res = (ResourceGraphScript*)App->resources->Get(uuid);
						res->LoadToMemory();
						//Name
						strcpy(script_name, App->resources->names[uuid].c_str());
					}
					else
						App->gui->AddLogToConsole("Couldn't charge the script to component");

				}
				ImGui::EndDragDropTarget();
			}

			//New script
			std::string new_script = "New Script" + cmp_idx;
			if (ImGui::Button(new_script.c_str(), { 80,30 })) {
				uuid_script = App->GenerateUUID();
				
				new_script_clicked = true;
				
				ResourceGraphScript* res = (ResourceGraphScript*)App->resources->CreateNewResource(Resource_Graph_Script, uuid_script);
				res->LoadToMemory();
			}
		}
		//If a graph node is already associated with this component
		else {
			std::string graph_visib_str = "";
			if (!show_graph)
				graph_visib_str = "Show Graph" + cmp_idx;
			else
				graph_visib_str = "Hide Graph" + cmp_idx;

			if (ImGui::Button(graph_visib_str.c_str(), { 80,30 })) {
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

uint ComponentGraphScript::GetCompSriptNum() const
{
	return script_num;
}

std::vector<GameObject*> ComponentGraphScript::GetBlackboard() const
{
	return BB_objects;
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

void ComponentGraphScript::ForceAddReferenceToBlackboard(GameObject * ref)
{
	BB_objects.push_back(ref);
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

void ComponentGraphScript::NewScriptMenu()
{

	ImGui::OpenPopup("Enter script name");

	if (ImGui::BeginPopupModal("Enter script name", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

		ImGui::InputText("##NewScriptName", script_name, 256);

		if (ImGui::Button("OK", ImVec2(200, 0))) {
			new_script_clicked = false;
			App->resources->names[uuid_script] = script_name;
			SaveScriptFile(uuid_script);

			//Add to assets panel, to use during the drag and drop
			string scr_name_ext = script_name + (string)".script";
			App->gui->GetPanelAssets()->AddToScriptsMap(scr_name_ext, uuid_script);
			
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
	
		ImGui::EndPopup();

	}

}

uint ComponentGraphScript::CreateNewTimer()
{
	timers.push_back(0.0f);
	return timers.size();
}

void ComponentGraphScript::IncrementTimer(uint idx, float dt)
{
	timers[idx - 1] += dt;
}

float ComponentGraphScript::GetTimer(uint idx) const
{
	return timers[idx - 1];
}

uint ComponentGraphScript::GetNumTimers() const
{
	return timers.size();
}

void ComponentGraphScript::Save(JSON_Array* comp_array) const {

	JSON_Value* value = json_value_init_object();
	JSON_Object* obj = json_value_get_object(value);

	json_object_set_number(obj, "Component Type", type);
	json_object_set_number(obj, "Script UUID", uuid_script);
	json_object_set_string(obj, "Name", script_name);
	json_object_set_boolean(obj, "Active", active);

	//Save Blackboard
	JSON_Value* value_arr = json_value_init_array();
	JSON_Array* array = json_value_get_array(value_arr);

	for (uint i = 1; i < BB_objects.size(); i++) {

		JSON_Value* aux_val = json_value_init_object();
		JSON_Object* aux_obj = json_value_get_object(aux_val);

		GameObject* it = BB_objects[i];

		json_object_set_number(aux_obj, "GO UUID", it->uuid);

		json_array_append_value(array, aux_val);
	}

	json_object_set_value(obj, "Blackboard", value_arr);

	json_array_append_value(comp_array, value);

	//Save .script file with all the info of nodes and links
	SaveScriptFile(uuid_script);
		
}



void ComponentGraphScript::Load(JSON_Object* comp_obj) {

	uuid_script = json_object_get_number(comp_obj, "Script UUID");
	active = json_object_get_boolean(comp_obj, "Active");
	std::strcpy(script_name, json_object_get_string(comp_obj, "Name"));
	
	ResourceGraphScript* res = (ResourceGraphScript*)App->resources->Get(uuid_script);

	if (res)
		res->LoadToMemory();

	//Load Blackboard
	JSON_Array* array_bb = json_object_get_array(comp_obj, "Blackboard");

	JSON_Object* it;

	for (uint i = 0; i < json_array_get_count(array_bb); i++) {

		it = json_array_get_object(array_bb, i);
		UID uuid_aux = json_object_get_number(it, "GO UUID");

		if (uuid_aux != 0) 
			BB_uids_load.push_back(uuid_aux);
		
	}

}


void ComponentGraphScript::SaveScriptFile(UID uuid) const{

	if (uuid != 0) {

		//std::string file = ASSETS_SCRIPT_FOLDER + std::to_string(uuid) + ".script";
		std::string name = script_name;
		std::string file = ASSETS_SCRIPT_FOLDER + name + ".script";

		JSON_Value* value = json_value_init_object();
		JSON_Object* obj = json_value_get_object(value);

		ResourceGraphScript* res = (ResourceGraphScript*)App->resources->Get(uuid_script);

		JSON_Value* value_arr_node = json_value_init_array();
		JSON_Array* array_nodes = json_value_get_array(value_arr_node);

		JSON_Value* value_arr_link = json_value_init_array();
		JSON_Array* array_links = json_value_get_array(value_arr_link);


		json_object_set_number(obj, "UUID", uuid);
		json_object_set_number(obj, "Resource Type", type);
		json_object_set_string(obj, "Name", name.c_str());

		res->node_graph->SaveFile(array_nodes, array_links);

		json_object_set_value(obj, "Nodes", value_arr_node);
		json_object_set_value(obj, "Links", value_arr_link);

		json_serialize_to_file_pretty(value, file.c_str());

		json_value_free(value);
	}


}


void ComponentGraphScript::LoadBlackBoard() {


	for (uint i = 0; i < BB_uids_load.size(); i++) {

		GameObject* bb_go = App->scene->GetGameObjectFromUUID(BB_uids_load[i], App->scene->GetRootGameObject());
		BB_objects.push_back(bb_go);

	}

	BB_uids_load.clear();

}