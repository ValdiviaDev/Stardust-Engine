#include "Application.h"
#include "NodeToggleActiveObject.h"
#include "GameObject.h"
#include "ModuleGui.h"


NodeToggleActiveObject::NodeToggleActiveObject(int id, const ImVec2& pos) : Node(id, "Action: ToggleActiveObject", pos, 1, 1, Node_Type_Action, Func_ToggleActiveObject)
{
}


NodeToggleActiveObject::~NodeToggleActiveObject()
{
}

bool NodeToggleActiveObject::Update(float dt, std::vector<GameObject*> BB_objects)
{
	node_state = Node_State_Idle;

	//If reference gets deleted, reference is the original object
	if (obj_indx >= BB_objects.size()) {
		obj_using_this = true;
		obj_indx = 0;
	}

	if (dt == 0.0f) {
		node_state = Node_State_ToUpdate;
		return true; //Show that node is updating, but nothing happens, because the simulation is paused
	}

	if (BB_objects[obj_indx] != nullptr) {
		switch (active_set) {
		case Toggle:
			BB_objects[obj_indx]->SetActive(!BB_objects[obj_indx]->IsActive());
			break;

		case True:
			BB_objects[obj_indx]->SetActive(true);
			break;

		case False:
			BB_objects[obj_indx]->SetActive(false);
			break;
		}

		node_state = Node_State_Updating;
	}
	else {
		node_state = Node_State_Error;
		App->gui->AddLogToConsole("ERROR: Can't toggle active: Object is NULL");
	}

	return true;
}

void NodeToggleActiveObject::Draw(std::vector<GameObject*> BB_objects)
{
	//GameObject reference
	DrawObjectsInstance(BB_objects);

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
