#include "Application.h"
#include "ModuleGui.h"
#include "NodeMoveObject.h"
#include "GameObject.h"
#include "ComponentTransform.h"


NodeMoveObject::NodeMoveObject(int id, const ImVec2& pos) : Node(id, "Action: MoveObject", pos, 1, 1, Node_Type_Action, Func_MoveObject)
{
	for (int i = 0; i < 3; ++i)
		direction[i] = 0;
}


NodeMoveObject::~NodeMoveObject()
{
}

bool NodeMoveObject::Update(float dt, std::vector<GameObject*> BB_objects)
{
	updating = false;
	if (BB_objects[obj_indx]) {
		updating = true;
		ComponentTransform* trans = (ComponentTransform*)BB_objects[obj_indx]->GetComponent(Comp_Transform);

		float3 new_pos = { direction[0] * velocity * dt, direction[1] * velocity * dt, direction[2] * velocity * dt };

		trans->SumPosition(new_pos);
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
		if (ImGui::BeginCombo("GameObject reference", BB_objects[obj_indx]->GetName())) {
			for (int i = 1; i < BB_objects.size(); ++i) {
				if (ImGui::Selectable(BB_objects[i]->GetName()))
					obj_indx = i;
			}

			ImGui::EndCombo();
		}
	}

	//Direction
	if (ImGui::InputInt3("Direction", direction)) {
		for (int i = 0; i < 3; ++i) {
			//Clamp vector
			if (direction[i] != 0 && direction[i] != 1 && direction[i] != -1) {
				if (direction[i] > 0)
					direction[i] = 1;
				else
					direction[i] = -1;
			}
		}
	}


	ImGui::InputFloat("Velocity", &velocity, 0.0f, 0.0f, 1);
}
