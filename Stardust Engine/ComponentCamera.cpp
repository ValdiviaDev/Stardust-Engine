
#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "Application.h"
#include "imgui/imgui.h"
#include "GameObject.h"
#include "Glew/include/glew.h"


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

void ComponentCamera::Update() {

	if (gameObject && gameObject->transform) {
		ComponentTransform* transform = gameObject->transform;
		math::float4x4 global_mat = transform->GetGlobalMatrix();

		frustum.pos = transform->GetGlobalPos();
		frustum.front = global_mat.WorldZ().Normalized();
		frustum.up = global_mat.WorldY().Normalized();
	}
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
	

}


void ComponentCamera::DrawFrustum() {

	glBegin(GL_LINES);
	glLineWidth(2.0f);
	glColor4f(1.0f, 0.5f, 0.0f, 1.0f);

	for (uint i = 0; i < frustum.NumEdges(); i++){

		glVertex3f(frustum.Edge(i).a.x, frustum.Edge(i).a.y, frustum.Edge(i).a.z);
		glVertex3f(frustum.Edge(i).b.x, frustum.Edge(i).b.y, frustum.Edge(i).b.z);
	}

	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}