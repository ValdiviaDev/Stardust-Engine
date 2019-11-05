
#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
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

	if (App->scene->GetRootGameObject()) {
		CameraCulling(App->scene->GetRootGameObject());
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

	if (near_plane >= frustum.farPlaneDistance)
		near_plane = frustum.farPlaneDistance - 1.0f;

	if (near_plane < 0.0f)
		near_plane = 0.1f;

	frustum.nearPlaneDistance = near_plane;
}


float ComponentCamera::GetNearPlane() const {

	return frustum.nearPlaneDistance;
}


void ComponentCamera::SetFarPlane(float far_plane) {

	if (far_plane <= frustum.nearPlaneDistance)
		far_plane = frustum.nearPlaneDistance + 1.0f;

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


void ComponentCamera::CameraCulling(GameObject* go) {

	if (!go->camera) {
		for (std::vector<GameObject*>::const_iterator it = go->childs.begin(); it < go->childs.end(); it++) {

			AABB refBox = (*it)->bounding_box;

			if (refBox.IsFinite() && go->mesh && go->mesh->m_info.num_vertex > 0) {

				
				//if (!frustum.Intersects(refBox)) {    //MathGeoLib func. Slow but works better
				if (ContainsAABB(refBox) == OUTSIDE) {
				
					go->SetActive(false);
				}
				else
					go->SetActive(true);

			}

			CameraCulling(*it);

		}
	}
}



// tests if a AaBox is within the frustrum
int ComponentCamera::ContainsAABB(const AABB& refBox) const
{
	math::float3 vCorner[8];
	int iTotalIn = 0;
	refBox.GetCornerPoints(vCorner); // get the corners of the box into the vCorner array
	// test all 8 corners against the 6 sides
	// if all points are behind 1 specific plane, we are out
	// if we are in with all points, then we are fully in
	math::Plane m_plane[6];
	frustum.GetPlanes(m_plane); //{ near, far, left, right, top, bottom }.
	
	for (int p = 0; p < 6; ++p) {
		int iInCount = 8;
		int iPtIn = 1;
		for (int i = 0; i < 8; ++i) {
			// test this point against the planes
			
			if (m_plane[p].IsOnPositiveSide(vCorner[i])){//m_plane[p].SideOfPlane(vCorner[i]) == BEHIND) {
				iPtIn = 0;
				--iInCount;
			}
		}
		// were all the points outside of plane p?
		if(iInCount == 0)
			return(OUTSIDE);
		// check if they were all on the right side of the plane
		iTotalIn += iPtIn;
	}
	// so if iTotalIn is 6, then all are inside the view
	if (iTotalIn == 6)
		return(INSIDE);
	// we must be partly in then otherwise
	return(INTERSECT);
}