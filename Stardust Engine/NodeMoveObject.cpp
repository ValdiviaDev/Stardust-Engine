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
	node_state = Node_State_Idle;
	GameObject* object = nullptr;

	static float3 dir = math::float3::zero;
	static bool local_dir_changed = false;

	//If reference gets deleted, send error
	if (obj_indx >= BB_objects.size())
		node_state = Node_State_Error;
	else
		object = BB_objects[obj_indx];


	if (object) {
		node_state = Node_State_Updating;
		ComponentTransform* trans = (ComponentTransform*)object->GetComponent(Comp_Transform);
		
		if (move_global) {
			dir = { (float)direction[0] , (float)direction[1] ,(float)direction[2] };
			trans->SumPositionGlobal(dir * velocity * dt);
		}

		else {//Move Local

			//If we don't change directions, we simply sum positions
			if (dont_change_dir_local && local_dir_changed)
				trans->SumPositionGlobal(dir * velocity * dt);

			//If we need to change directions, we calculate the direction vector every frame
			else {
				dir = { (float)direction[0] , (float)direction[1], (float)direction[2] };
				trans->SumPositionLocal(dir, velocity * dt);

				local_dir_changed = true;
			}
		}
	}
	else
		App->gui->AddLogToConsole("ERROR: Can't move object: Object is NULL");

	return true;
}

void NodeMoveObject::Draw(std::vector<GameObject*> BB_objects)
{
	//GameObject reference
	DrawObjectsInstance(BB_objects);

	//Move the object in global space or local space
	if (ImGui::RadioButton("Global", move_global == true))
		move_global = true;
	ImGui::SameLine();
	if (ImGui::RadioButton("Local", move_global == false))
		move_global = false;

	if (!move_global)
		ImGui::Checkbox("Don't change direction", &dont_change_dir_local);

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
			direction[0] = 0; direction[1] = -1; direction[2] = 0;
			dir_str = "Up";
		}

		if (ImGui::Selectable("Down")) {
			direction[0] = 0; direction[1] = 1; direction[2] = 0;
			dir_str = "Down";
		}

		ImGui::EndCombo();
	}

	//Velocity
	ImGui::InputFloat("Velocity", &velocity, 0.0f, 0.0f, 1);
}


void NodeMoveObject::Save() const {

}

void NodeMoveObject::Load() {

}