#include "Application.h"
#include "ModuleGui.h"
#include "NodeMoveObject.h"
#include "GameObject.h"
#include "ComponentTransform.h"


NodeMoveObject::NodeMoveObject(int id, const ImVec2& pos) : Node(id, "Action: MoveObject", pos, 1, 0, Node_Type_Function, Func_MoveObject)
{
	for (int i = 0; i < 3; ++i)
		direction[i] = 0;
}


NodeMoveObject::~NodeMoveObject()
{
}

bool NodeMoveObject::Update(float dt, GameObject* object)
{
	updating = false;

	ComponentTransform* trans = (ComponentTransform*)object->GetComponent(Comp_Transform);

	if (trans)
		updating = true;

	float3 new_pos = { direction[0] * velocity, direction[1] * velocity, direction[2] * velocity };

	trans->SumPosition(new_pos);

	return updating;
}

void NodeMoveObject::Draw()
{
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
