#include "Application.h"
#include "NodeToggleActiveComp.h"
#include "NodeToggleActiveObject.h"
#include "GameObject.h"
#include "Component.h"
#include "ModuleGui.h"



NodeToggleActiveComp::NodeToggleActiveComp(int id, const ImVec2& pos) : Node(id, "Action: ToggleActiveComp", pos, 1, 1, Node_Type_Action, Func_ToggleActiveComp)
{
	active_set = Toggle;
	comp_type = Comp_Mesh;
}


NodeToggleActiveComp::~NodeToggleActiveComp()
{
}

bool NodeToggleActiveComp::Update(float dt, std::vector<GameObject*> BB_objects, uint num_comp_graph)
{
	node_state = Node_State_Idle;
	GameObject* object = nullptr;

	if (dt == 0.0f) {
		node_state = Node_State_ToUpdate;
		return true; //Show that node is updating, but nothing happens, because the simulation is paused
	}

	//If reference gets deleted, send error
	if (obj_indx >= BB_objects.size())
		node_state = Node_State_Error;
	else
		object = BB_objects[obj_indx];


	if (object) {
		Component* comp = object->GetComponent(comp_type);
		
		if (comp) {
			switch (active_set) {
			case Toggle:
				comp->SetActive(!comp->IsActive());
				break;

			case True:
				comp->SetActive(true);
				break;

			case False:
				comp->SetActive(false);
				break;
			}

			node_state = Node_State_Updating;
		}
		else
			App->gui->AddLogToConsole("ERROR: Can't toggle active component: Component is NULL");
	}
	else {
		node_state = Node_State_Error;
		App->gui->AddLogToConsole("ERROR: Can't toggle active component: Object is NULL");
	}

	return true;
}

void NodeToggleActiveComp::Draw(std::vector<GameObject*> BB_objects)
{
	//GameObject reference
	DrawObjectsInstance(BB_objects);

	//Component to toggle
	static const char* comp_set_str[] = { "Mesh", "Material", "Script" };

	if (ImGui::BeginCombo("Set component", comp_str)) {
		if (ImGui::Selectable("Mesh")) {
			comp_str = comp_set_str[0];
			comp_type = Comp_Mesh;
		}

		if (ImGui::Selectable("Material")) {
			comp_str = comp_set_str[1];
			comp_type = Comp_Material;
		}
		
		if (ImGui::Selectable("Graph Script")) {
			comp_str = comp_set_str[2];
			comp_type = Comp_Graph_Script;
		}

		ImGui::EndCombo();
	}

	//Set active options
	static const char* active_set_str[] = { "Toggle", "True", "False" };

	if (ImGui::BeginCombo("Set active", curr_active_set_str)) {

		if (ImGui::Selectable("Toggle")) {
			curr_active_set_str = active_set_str[0];
			active_set = Toggle;
		}
		if (ImGui::Selectable("True")) {
			curr_active_set_str = active_set_str[1];
			active_set = True;
		}
		if (ImGui::Selectable("False")) {
			curr_active_set_str = active_set_str[2];
			active_set = False;
		}

		ImGui::EndCombo();
	}

}

void NodeToggleActiveComp::Save(JSON_Object* obj) const {

	json_object_set_boolean(obj, "using this", obj_using_this);
	json_object_set_number(obj, "id using", obj_indx);
	json_object_set_string(obj, "curr active set str", curr_active_set_str);
	json_object_set_string(obj, "comp str", comp_str);
	json_object_set_number(obj, "active set", active_set);
	json_object_set_number(obj, "comp type", comp_type);

	
}

void NodeToggleActiveComp::Load(JSON_Object* obj) {

	obj_using_this = json_object_get_boolean(obj, "using this");
	obj_indx = json_object_get_number(obj, "id using");
	curr_active_set_str = json_object_get_string(obj, "curr active set str");
	comp_str = json_object_get_string(obj, "comp str");
	active_set = (ActiveSetTo)((int)json_object_get_number(obj, "active set"));
	comp_type = (ComponentType)((int)json_object_get_number(obj, "comp type"));

}