#include "ComponentTransform.h"
#include "Globals.h"
#include "GameObject.h"
#include "imgui/imgui.h"
#include "MathGeoLib/include/MathGeoLib.h"

ComponentTransform::ComponentTransform(GameObject* parent) : Component(parent)
{
	LOG("Added Transform to GO");
	type = Comp_Transform;
}


ComponentTransform::~ComponentTransform()
{
}


void ComponentTransform::DrawInspector() {

	//if (ImGui::TreeNodeEx("Transform")) {
	if (ImGui::CollapsingHeader("Transform")) {

		ImGui::Text("Position");
		ImGui::SameLine();
		ImGui::DragFloat3("p", &position[0], 0.1f);

		ImGui::Text("Rotation");
		ImGui::SameLine();
		if (ImGui::DragFloat3("r", &rotation[0], 0.1f)) {
			SetRotation(rotation);
		}

		ImGui::Text("Scale   ");
		ImGui::SameLine();
		ImGui::DragFloat3("s", &scale[0], 0.1f);

	
	}
	
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

math::float3 ComponentTransform::GetPosition() const{

	return position;
}

math::float3 ComponentTransform::GetRotation() const{

	return rotation;
}

math::float3 ComponentTransform::GetScale() const{

	return scale;
}




void ComponentTransform::UpdateMatrix() {
	
	local_matrix = math::float4x4::FromTRS(position, quaternion_rot, scale);

	GameObject* parent_aux = gameObject->GetParent();
	if (parent_aux != nullptr && parent_aux->GetParent() != nullptr) {  
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


math::float3 ComponentTransform::GetGlobalPos() const {

	math::float3 ret = position;

	GameObject* aux = gameObject->GetParent();
	

	while (aux != nullptr) {
		
		if (aux->transform != nullptr) {
			ret += aux->transform->GetPosition();

		}
		aux = aux->GetParent();
	}

	return ret;
}