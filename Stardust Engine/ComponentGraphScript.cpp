#include "Application.h"
#include "ComponentGraphScript.h"
#include "ModuleResourceManager.h"
#include "ResourceGraphScript.h"
#include "NodeGraph.h"
#include "GameObject.h"
#include "ModuleGui.h"
#include "ModuleScene.h"
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




void ComponentGraphScript::Save(JSON_Array* comp_array) const {

	JSON_Value* value = json_value_init_object();
	JSON_Object* obj = json_value_get_object(value);

	json_object_set_number(obj, "Component Type", type);
	json_object_set_number(obj, "Script UUID", uuid_script);


	//Save Blackboard
	JSON_Value* value_arr = json_value_init_array();
	JSON_Array* array = json_value_get_array(value_arr);

	for (uint i = 0; i < BB_objects.size(); i++) {

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

	
	ResourceGraphScript* res = (ResourceGraphScript*)App->resources->Get(uuid_script);

	if (res) {
		
		res->LoadToMemory();
		//res->node_graph->LoadFile(uuid_script);
		
	}


	//Load Blackboard
	JSON_Array* array_bb = json_object_get_array(comp_obj, "Blackboard");

	JSON_Object* it;

	for (uint i = 0; i < json_array_get_count(array_bb); i++) {

		it = json_array_get_object(array_bb, i);
		UID uuid_aux = json_object_get_number(it, "GO UUID");

		if (uuid_aux != 0) {
			GameObject* bb_go = App->scene->GetGameObjectFromUUID(uuid_aux, App->scene->GetRootGameObject());
			BB_objects.push_back(bb_go);
		}
	}

}


void ComponentGraphScript::SaveScriptFile(UID uuid) const{

	if (uuid != 0) {

		std::string file = ASSETS_SCRIPT_FOLDER + std::to_string(uuid) + ".script";

		//JSON_Value* root_value = json_value_init_array();
		//JSON_Array* array = json_value_get_array(root_value);

		JSON_Value* value = json_value_init_object();
		JSON_Object* obj = json_value_get_object(value);

		ResourceGraphScript* res = (ResourceGraphScript*)App->resources->Get(uuid_script);

		JSON_Value* value_arr_node = json_value_init_array();
		JSON_Array* array_nodes = json_value_get_array(value_arr_node);

		JSON_Value* value_arr_link = json_value_init_array();
		JSON_Array* array_links = json_value_get_array(value_arr_link);


		json_object_set_number(obj, "UUID", uuid);
		json_object_set_number(obj, "Resource Type", type);

		res->node_graph->SaveFile(array_nodes, array_links);

		json_object_set_value(obj, "Nodes", value_arr_node);
		json_object_set_value(obj, "Links", value_arr_link);

		json_serialize_to_file_pretty(value, file.c_str());

		json_value_free(value);
	}


}


