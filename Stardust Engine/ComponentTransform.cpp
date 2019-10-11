#include "ComponentTransform.h"
#include "Globals.h"
#include "GameObject.h"

ComponentTransform::ComponentTransform(GameObject* parent) : Component(parent)
{
	LOG("Added Transform to GO");
	type = Comp_Transform;
}


ComponentTransform::~ComponentTransform()
{
}


void ComponentTransform::SetPosition(math::float3 pos) {

	position = pos;
}

void ComponentTransform::SetScale(math::float3 scale) {

	this->scale = scale;
}

void ComponentTransform::SetRotation(math::float3 rot) {

	rotation = rot;
	quaternion_rot = math::Quat::FromEulerXYZ(rotation.x * DEGTORAD, rotation.y * DEGTORAD, rotation.z * DEGTORAD);
}

math::float3 ComponentTransform::GetPosition() {

	return position;
}

math::float3 ComponentTransform::GetRotation() {

	return rotation;
}

math::float3 ComponentTransform::GetScale() {

	return scale;
}




void ComponentTransform::UpdateMatrix() {
	
	local_matrix = math::float4x4::FromTRS(position, quaternion_rot, scale);

	GameObject* parent_aux = p_GameObject->GetParent();
	if (parent_aux != nullptr){// && parent_aux->GetParent() != nullptr) {  //UNCOMMENT THIS WHEN WE STOP USING THE ROOT GAME OBJECT FOR TEST
		ComponentTransform* p_transform = parent_aux->transform;

		if (p_transform != nullptr) {

			math::float4x4 p_global_matrix = p_transform->GetGlobalMatrix();

			global_matrix = p_global_matrix * local_matrix;

		}
		else {
			global_matrix = local_matrix;
		}

	}else {
		global_matrix = local_matrix;
	}

}



math::float4x4 ComponentTransform::GetGlobalMatrix() const {

	return global_matrix;
}