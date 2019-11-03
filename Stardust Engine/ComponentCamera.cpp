
#include "ComponentCamera.h"
#include "Application.h"
#include "imgui/imgui.h"



ComponentCamera::ComponentCamera(GameObject* game_object) :Component(game_object) {

	type = Comp_Camera;
	aspect_ratio = 1.7778f;   // 16 / 9
	frustum.type = math::PerspectiveFrustum;
	frustum.front = math::float3::unitZ;
	frustum.up = math::float3::unitY;
	frustum.pos = math::float3::zero;
	frustum.nearPlaneDistance = 1.0f;
	frustum.farPlaneDistance = 100.0f;
	frustum.verticalFov = DEGTORAD *60;
	frustum.horizontalFov = 2.0f * atanf(aspect_ratio * tanf(frustum.verticalFov * 0.5f));
}


ComponentCamera::~ComponentCamera() {

}

float ComponentCamera::GetFOV() const {

	return RADTODEG * frustum.verticalFov;
}


void ComponentCamera::SetFOV(float fov) {

	frustum.verticalFov = DEGTORAD * fov;
	frustum.horizontalFov = 2.0f * atanf(aspect_ratio * tanf(frustum.verticalFov * 0.5f));

}


void ComponentCamera::SetNearPlane(float near_plane) {

	frustum.nearPlaneDistance = near_plane;
}


float ComponentCamera::GetNearPlane() const {

	return frustum.nearPlaneDistance;
}


void ComponentCamera::SetFarPlane(float far_plane) {
	
	frustum.farPlaneDistance = far_plane;
}


float ComponentCamera::GetFarPlane() const {

	return frustum.farPlaneDistance;
}


void ComponentCamera::DrawInspector() {


	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{

		ImGui::Text("Horizontal FOV:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.0f, 1.0f), "%f", frustum.horizontalFov);


		ImGui::Text("Vertical FOV:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.0f, 1.0f), "%f", frustum.verticalFov);


		float fov = frustum.verticalFov * RADTODEG;
		ImGui::Text("FOV");
		ImGui::SameLine();
		if (ImGui::DragFloat("fov", &fov, 1.0f))
			SetFOV(fov);


		float near_plane = frustum.nearPlaneDistance;
		ImGui::Text("Near Plane:");
		ImGui::SameLine();
		if (ImGui::DragFloat("near", &near_plane, 1.0F))
		{
			SetNearPlane(near_plane);
		}


		float far_plane = frustum.farPlaneDistance;
		ImGui::Text("Far Plane:");
		ImGui::SameLine();
		if (ImGui::DragFloat("far", &far_plane, 1.0F))
		{
			SetFarPlane(far_plane);
		}
		
	}
	/*

	ImGui::Text("Rotation");
		ImGui::SameLine();
		if (ImGui::DragFloat3("  ", &rotation[0], 0.1f)) {
			SetRotation(rotation);
		}



	if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("Mesh path: ");
		ImGui::SameLine();

		if(path)
			ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.0f, 1.0f), path);
		else
			ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "null");

		ImGui::Separator();

		if (!is_primitive) {
		ImGui::Text("Debug Options");

			ImGui::Checkbox("Vertex Normals", &debug_v_norm);
			ImGui::SameLine();
			ImGui::Checkbox("Face Normals", &debug_f_norm);
		}

	}

	*/

}


