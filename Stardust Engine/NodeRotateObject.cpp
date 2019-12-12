#include "NodeRotateObject.h"
#include "GameObject.h"
#include "ComponentTransform.h"


NodeRotateObject::NodeRotateObject(int id, const ImVec2& pos) : Node(id, "Action: RotateObject", pos, 1, 1, Node_Type_Action, Func_RotateObject)
{
	rot_vel = { 0.0f,0.0f,0.0f };
}

NodeRotateObject::~NodeRotateObject()
{
}

bool NodeRotateObject::Update(float dt, std::vector<GameObject*> BB_objects)
{
	updating = false;
	if (BB_objects[obj_indx]) {
		updating = true;
		ComponentTransform* trans = (ComponentTransform*)BB_objects[obj_indx]->GetComponent(Comp_Transform);
		
		if(!rot_with_mouse)
			trans->SumRotation(rot_vel);
	}

	return updating;
}

void NodeRotateObject::Draw(std::vector<GameObject*> BB_objects)
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

	ImGui::Checkbox("Rot with mouse", &rot_with_mouse);
	if (!rot_with_mouse) {
		//Normal rotation
		ImGui::Text("Rot velocity (degrees)");
		ImGui::DragFloat3(" ", &rot_vel[0], 0.1f);
	}
}
