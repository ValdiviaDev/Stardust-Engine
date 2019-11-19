#include "ComponentTransform.h"
#include "Globals.h"
#include "Application.h"
#include "Quadtree.h"
#include "GameObject.h"
#include "ComponentCamera.h"
#include "imgui/imgui.h"
#include "imgui/ImGuizmo.h"
#include "MathGeoLib/include/MathGeoLib.h"



ComponentTransform::ComponentTransform(GameObject* parent) : Component(parent)
{
	LOG("Added Transform to GO");
	type = Comp_Transform;
}


ComponentTransform::~ComponentTransform()
{
	if (gameObject)
		gameObject->transform = nullptr;
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

	if (App->GetEngineState() == Engine_State_Editor)
		HandleGizmos();
}

void ComponentTransform::HandleGizmos()
{
	//TODO guizmos
	ImGuizmo::Enable(true);

	static bool draw_guizmo = false;
	static ImGuizmo::OPERATION current_operation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE current_mode(ImGuizmo::WORLD);

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_IDLE) {
		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN) //Selected
			draw_guizmo = false;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) { //Translate
			draw_guizmo = true;
			current_operation = ImGuizmo::TRANSLATE;
		}
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN) { //Rotate
			draw_guizmo = true;
			current_operation = ImGuizmo::ROTATE;
		}
		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN) { //Scale
			draw_guizmo = true;
			current_operation = ImGuizmo::SCALE;
		}
		if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN) { //Change global to world and backwards
			if(current_mode == ImGuizmo::MODE::WORLD)
				current_mode = ImGuizmo::MODE::LOCAL;
			else
				current_mode = ImGuizmo::MODE::WORLD;
		}
	}

	if (draw_guizmo) {
		float* view_mat = App->camera->current_cam->GetViewMatrix();
		float* proj_mat = App->camera->current_cam->GetProjectionMatrix();
		float4x4 this_mat = global_matrix.Transposed();

		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
		ImGuizmo::Manipulate(view_mat, proj_mat, current_operation, current_mode, (float*)&this_mat);
	
		
		if (ImGuizmo::IsUsing()) {
			//Put the values of the matrix in each corresponding vector/quaternion
			this_mat.Transpose();
			GameObject* parent = gameObject->GetParent();
			
			//If the GameObject does have a parent, we have to recalculate the local matrix (root_node->parent is always null!!)
			if (parent->GetParent() != nullptr)
				this_mat = parent->transform->GetGlobalMatrix().Inverted() * this_mat;
			
			if (current_operation != ImGuizmo::SCALE) {
				this_mat.Decompose(position, quaternion_rot, scale);

				if (current_operation == ImGuizmo::ROTATE)
					SetRotationFromQuat();
			}
			else {
				//current_mode = ImGuizmo::MODE::LOCAL;
				float3 disposable_pos;
				Quat disposable_rot;
				this_mat.Decompose(disposable_pos, disposable_rot, scale);
			}

		}
	
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

void ComponentTransform::SetRotationFromQuat() {
	math::Quat deg_quat_rot = { quaternion_rot.x, quaternion_rot.y, quaternion_rot.z, quaternion_rot.w};
	rotation = deg_quat_rot.ToEulerXYZ();
	rotation *= RADTODEG;
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
			App->scene->EraseObjFromStatic(gameObject);
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

void ComponentTransform::SetTransformFromParent()
{
	float4x4 mat = gameObject->GetParent()->transform->global_matrix.Inverted() * global_matrix;

	mat.Decompose(position, quaternion_rot, scale);
	SetRotationFromQuat();
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

void ComponentTransform::Load(JSON_Object* comp_obj) {

	position.x = json_object_get_number(comp_obj, "PosX");
	position.y = json_object_get_number(comp_obj, "PosY");
	position.z = json_object_get_number(comp_obj, "PosZ");
	quaternion_rot.x = json_object_get_number(comp_obj, "RotX");
	quaternion_rot.y = json_object_get_number(comp_obj, "RotY");
	quaternion_rot.z = json_object_get_number(comp_obj, "RotZ");
	quaternion_rot.w = json_object_get_number(comp_obj, "RotW");
	scale.x = json_object_get_number(comp_obj, "ScaleX");
	scale.y = json_object_get_number(comp_obj, "ScaleY");
	scale.z = json_object_get_number(comp_obj, "ScaleZ");

	SetRotationFromQuat();
}