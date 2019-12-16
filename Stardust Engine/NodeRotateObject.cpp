#include "Application.h"
#include "NodeRotateObject.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ModuleInput.h"
#include "ModuleGui.h"


NodeRotateObject::NodeRotateObject(int id, const ImVec2& pos) : Node(id, "Action: RotateObject", pos, 1, 1, Node_Type_Action, Func_RotateObject)
{
	rot_vel = { 0.0f,0.0f,0.0f };
}

NodeRotateObject::~NodeRotateObject()
{
}

bool NodeRotateObject::Update(float dt, std::vector<GameObject*> BB_objects)
{
	node_state = Node_State_Idle;

	//If reference gets deleted, reference is the original object
	if (obj_indx >= BB_objects.size()) {
		obj_using_this = true;
		obj_indx = 0;
	}

	if (BB_objects[obj_indx]) {
		node_state = Node_State_Updating;
		ComponentTransform* trans = (ComponentTransform*)BB_objects[obj_indx]->GetComponent(Comp_Transform);
		
		if (rot_with_mouse) {
			float mouse_motion = -App->input->GetMouseXMotion();
			mouse_motion += App->input->GetMouseYMotion();
			//App->input->GetMouseX();

			math::float3 rot = ((rot_vel  * DEGTORAD) * (mouse_motion)) * dt;
			trans->SumRotation(rot * RADTODEG);
		}
		else
			trans->SumRotation(rot_vel * dt);

	}
	else {
		node_state = Node_State_Error;
		App->gui->AddLogToConsole("ERROR: Can't rotate object: Object is NULL");
	}

	return true;
}

void NodeRotateObject::Draw(std::vector<GameObject*> BB_objects)
{
	//GameObject reference
	DrawObjectsInstance(BB_objects);

	ImGui::Checkbox("Rot with mouse", &rot_with_mouse);

	//Normal rotation
	ImGui::Text("Rot velocity (degrees)");
	ImGui::DragFloat3(" ", &rot_vel[0], 0.1f);

}
