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
	GameObject* object = nullptr;

	//If reference gets deleted, send error
	if (obj_indx >= BB_objects.size())
		node_state = Node_State_Error;
	else
		object = BB_objects[obj_indx];


	if (object) {
		node_state = Node_State_Updating;
		ComponentTransform* trans = (ComponentTransform*)object->GetComponent(Comp_Transform);
		
		if (!reset_rot) {
			if (rot_with_mouse) {
				float mouse_motion = 0.0f;
				if(mouse_rot_X)
					mouse_motion += -App->input->GetMouseXMotion();
				if (mouse_rot_Y)
					mouse_motion += App->input->GetMouseYMotion();

				math::float3 rot = ((rot_vel  * DEGTORAD) * (mouse_motion)) * dt;
				trans->SumRotation(rot * RADTODEG);
			}
			else
				trans->SumRotation(rot_vel * dt);
		}
		else
			trans->SetRotation({ 0.0f,0.0f,0.0f });

	}
	else
		App->gui->AddLogToConsole("ERROR: Can't rotate object: Object is NULL");

	return true;
}

void NodeRotateObject::Draw(std::vector<GameObject*> BB_objects)
{
	//GameObject reference
	DrawObjectsInstance(BB_objects);
	if (!reset_rot) {
		ImGui::Checkbox("Rot with mouse", &rot_with_mouse);
		if (rot_with_mouse) {
			ImGui::Checkbox("Mouse X axis", &mouse_rot_X);
			ImGui::SameLine();
			ImGui::Checkbox("Mouse Y axis", &mouse_rot_Y);
		}

		//Normal rotation
		ImGui::Text("Rot velocity");
		ImGui::DragFloat3(" ", &rot_vel[0], 0.1f);
	}

	ImGui::Checkbox("Reset rotation", &reset_rot);

}


void NodeRotateObject::Save() const {

}

void NodeRotateObject::Load() {

}