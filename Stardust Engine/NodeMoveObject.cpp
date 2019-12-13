#include "Application.h"
#include "ModuleGui.h"
#include "NodeMoveObject.h"
#include "GameObject.h"
#include "ComponentTransform.h"


NodeMoveObject::NodeMoveObject(int id, const ImVec2& pos) : Node(id, "Action: MoveObject", pos, 1, 1, Node_Type_Action, Func_MoveObject)
{
	direction[0] = 0; direction[1] = 0; direction[2] = 1;
}


NodeMoveObject::~NodeMoveObject()
{
}

bool NodeMoveObject::Update(float dt, std::vector<GameObject*> BB_objects)
{
	updating = false;

	//If reference gets deleted, reference is the original object
	if (obj_indx >= BB_objects.size()) {
		obj_using_this = true;
		obj_indx = 0;
	}

	if (BB_objects[obj_indx]) {
		updating = true;
		ComponentTransform* trans = (ComponentTransform*)BB_objects[obj_indx]->GetComponent(Comp_Transform);

		float3 new_pos = { direction[0] * velocity * dt, direction[1] * velocity * dt, direction[2] * velocity * dt };

		trans->SumPosition(new_pos);

		error = false;
	}
	else {
		error = true;
		App->gui->AddLogToConsole("ERROR: Can't move object: Object is NULL");
	}

	return updating;
}

void NodeMoveObject::Draw(std::vector<GameObject*> BB_objects)
{
	//GameObject reference
	if (ImGui::Checkbox("This object", &obj_using_this)) {
		if (obj_using_this)
			obj_indx = 0;
	}

	if (!obj_using_this) {
		//If reference gets deleted, reference is the original object
		if (obj_indx >= BB_objects.size()) {
			obj_using_this = true;
			obj_indx = 0;
		}

		if (ImGui::BeginCombo("GameObject reference", BB_objects[obj_indx]->GetName())) {
			for (int i = 1; i < BB_objects.size(); ++i) {
				if (ImGui::Selectable(BB_objects[i]->GetName()))
					obj_indx = i;
			}

			ImGui::EndCombo();
		}
	}

	//Direction
	if (ImGui::BeginCombo("Direction", dir_str)) {
		if (ImGui::Selectable("Forward")) {
			direction[0] = 0; direction[1] = 0; direction[2] = 1;
			dir_str = "Forward";
		}

		if (ImGui::Selectable("Backward")) {
			direction[0] = 0; direction[1] = 0; direction[2] = -1;
			dir_str = "Backward";
		}

		if (ImGui::Selectable("Left")) {
			direction[0] = -1; direction[1] = 0; direction[2] = 0;
			dir_str = "Left";
		}

		if (ImGui::Selectable("Right")) {
			direction[0] = 1; direction[1] = 0; direction[2] = 0;
			dir_str = "Right";
		}

		if (ImGui::Selectable("Up")) {
			direction[0] = 0; direction[1] = 1; direction[2] = 0;
			dir_str = "Up";
		}

		if (ImGui::Selectable("Down")) {
			direction[0] = 0; direction[1] = -1; direction[2] = 0;
			dir_str = "Down";
		}

		ImGui::EndCombo();
	}

	//Velocity
	ImGui::InputFloat("Velocity", &velocity, 0.0f, 0.0f, 1);
}
