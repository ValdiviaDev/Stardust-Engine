#include "NodeToggleActiveObject.h"
#include "GameObject.h"


NodeToggleActiveObject::NodeToggleActiveObject(int id, const ImVec2& pos) : Node(id, "Action: ToggleActiveObject", pos, 1, 1, Node_Type_Action, Func_ToggleActiveObject)
{
}


NodeToggleActiveObject::~NodeToggleActiveObject()
{
}

bool NodeToggleActiveObject::Update(float dt, std::vector<GameObject*> BB_objects)
{
	updating = false;

	if (BB_objects[0]) {
		switch (active_set) {
		case Toggle:
			BB_objects[0]->SetActive(!BB_objects[0]->IsActive());
			break;

		case True:
			BB_objects[0]->SetActive(true);
			break;

		case False:
			BB_objects[0]->SetActive(false);
			break;
		}

		updating = true;
	}

	return false;
}

void NodeToggleActiveObject::Draw()
{
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
