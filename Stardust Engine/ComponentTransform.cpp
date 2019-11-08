#include "ComponentTransform.h"
#include "Globals.h"
#include "Application.h"
#include "Quadtree.h"
#include "GameObject.h"
#include "imgui/imgui.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "Parson/parson.h"



ComponentTransform::ComponentTransform(GameObject* parent) : Component(parent)
{
	LOG("Added Transform to GO");
	type = Comp_Transform;
}


ComponentTransform::~ComponentTransform()
{
}


void ComponentTransform::DrawInspector() {

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::Text("Position");
		ImGui::SameLine();
		ImGui::DragFloat3(" ", &position[0], 0.1f);

		ImGui::Text("Rotation");
		ImGui::SameLine();
		if (ImGui::DragFloat3("  ", &rotation[0], 0.1f)) {
			SetRotation(rotation);
		}

		ImGui::Text("Scale   ");
		ImGui::SameLine();
		ImGui::DragFloat3("   ", &scale[0], 0.1f);
	
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

	last_global_matrix = global_matrix;

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

	//If the transform propieties have changed in this frame
	if (!last_global_matrix.Equals(global_matrix)) {
		gameObject->UpdateBoundingBox();
		if (gameObject->IsStatic()) {
			gameObject->SetStatic(false);
			App->scene->quadtree->Remove(gameObject);
		}
	}
}



math::float4x4 ComponentTransform::GetGlobalMatrix() const {

	return global_matrix;
}

void ComponentTransform::SumPosition(math::float3 pos)
{
	position += pos;
}

void ComponentTransform::SumRotation(math::float3 rot)
{
	quaternion_rot = quaternion_rot * Quat::FromEulerXYZ(rot.x * DEGTORAD, rot.y * DEGTORAD, rot.z * DEGTORAD);
	rotation = quaternion_rot.ToEulerXYZ() * RADTODEG;
}

void ComponentTransform::SumScale(math::float3 scale)
{
	this->scale.x *= scale.x;
	this->scale.y *= scale.y;
	this->scale.z *= scale.z;
}

void ComponentTransform::Reset()
{
	SetPosition({ 0.0f, 0.0f, 0.0f });
	SetRotation({ 0.0f, 0.0f, 0.0f });
	SetScale({ 1.0f, 1.0f, 1.0f });
}


math::float3 ComponentTransform::GetGlobalPos() const {

	//math::float3 ret = position;

	//GameObject* aux = gameObject->GetParent();
	//

	//while (aux != nullptr) {
	//	
	//	if (aux->transform != nullptr) {
	//		ret += aux->transform->GetPosition();

	//	}
	//	aux = aux->GetParent();
	//}

	//return ret;

	return global_matrix.TranslatePart();
}



math::float3 ComponentTransform::GetGlobalScale() const {

	math::float3 ret = scale;

	GameObject* aux = gameObject->GetParent();


	while (aux != nullptr) {

		if (aux->transform != nullptr) {
			ret.x *= aux->transform->GetScale().x;
			ret.y *= aux->transform->GetScale().y;
			ret.z *= aux->transform->GetScale().z;
		}
		aux = aux->GetParent();
	}

	return ret;
}



void ComponentTransform::Save(JSON_Array* comp_array) {


	JSON_Value* value = json_value_init_object();
	JSON_Object* obj = json_value_get_object(value);

	json_object_set_number(obj, "Component Type", type);

	json_object_set_number(obj, "PosX", position.x);
	json_object_set_number(obj, "PosY", position.y);
	json_object_set_number(obj, "PosZ", position.z);
	json_object_set_number(obj, "RotX", quaternion_rot.x);
	json_object_set_number(obj, "RotY", quaternion_rot.y);
	json_object_set_number(obj, "RotZ", quaternion_rot.z);
	json_object_set_number(obj, "RotW", quaternion_rot.w);
	json_object_set_number(obj, "ScaleX", scale.x);
	json_object_set_number(obj, "ScaleY", scale.y);
	json_object_set_number(obj, "ScaleZ", scale.z);

	json_array_append_value(comp_array, value);

}