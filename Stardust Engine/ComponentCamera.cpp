#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "Application.h"
#include "imgui/imgui.h"
#include "GameObject.h"
#include "Quadtree.h"
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

	if(gameObject)
		gameObject->camera = nullptr;

}

void ComponentCamera::Update() {

	if (gameObject && gameObject->transform) {
		ComponentTransform* transform = gameObject->transform;
		math::float4x4 global_mat = transform->GetGlobalMatrix();

		frustum.pos = transform->GetGlobalPos();
		frustum.front = global_mat.WorldZ().Normalized();
		frustum.up = global_mat.WorldY().Normalized();
	}

	if (culling) {
		if (App->scene->GetRootGameObject()) {
			CameraCullingStObj();
			CameraCullingDynObj(App->scene->GetRootGameObject());
		}
	}
	
	glDisable(GL_LIGHTING);
	DrawFrustum();
	glEnable(GL_LIGHTING);
	
}


float ComponentCamera::GetFOV() const {

	return RADTODEG * frustum.verticalFov;
}


void ComponentCamera::SetFOV(float fov) {

	frustum.verticalFov = DEGTORAD * fov;
	frustum.horizontalFov = 2.0f * atanf(aspect_ratio * tanf(frustum.verticalFov * 0.5f));
}

void ComponentCamera::SetAspectRatio(float AR)
{
	aspect_ratio = AR;
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

		if (ImGui::Checkbox("Main Camera", &main_camera)) {
			if (main_camera)
				App->scene->SetMainCamera(this);
			else
				App->scene->SetMainCamera(nullptr);
		}

		ImGui::Text("Horizontal FOV:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.0f, 1.0f), "%f", frustum.horizontalFov);


		ImGui::Text("Vertical FOV:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.0f, 1.0f), "%f", frustum.verticalFov);


		float fov = frustum.verticalFov * RADTODEG;
		ImGui::Text("FOV");
		ImGui::SameLine();
		if (ImGui::DragFloat("fov", &fov, 1.0f)) {
			SetFOV(fov);
			App->renderer3D->RecalculateProjMat();
		}


		float near_plane = frustum.nearPlaneDistance;
		ImGui::Text("Near Plane:");
		ImGui::SameLine();
		if (ImGui::DragFloat("near", &near_plane, 1.0F))
		{
			SetNearPlane(near_plane);
			App->renderer3D->RecalculateProjMat();
		}


		float far_plane = frustum.farPlaneDistance;
		ImGui::Text("Far Plane:");
		ImGui::SameLine();
		if (ImGui::DragFloat("far", &far_plane, 1.0F))
		{
			SetFarPlane(far_plane);
			App->renderer3D->RecalculateProjMat();
		}
		
		if (ImGui::Checkbox("Camera Culling", &culling)) {
			if (!culling)
				App->scene->AllObjectsActive(App->scene->GetRootGameObject());

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


void ComponentCamera::CameraCullingStObj()
{
	vector<GameObject*> obj_to_cull;
	vector<GameObject*> st_obj = App->scene->static_objects;

	//All static objects not culling
	for (int i = 0; i < st_obj.size(); ++i)
		st_obj[i]->SetActive(false);

	App->scene->quadtree->Intersect(obj_to_cull, frustum);

	//Only cull objects provided by the quadtree
	for (int i = 0; i < obj_to_cull.size(); ++i)
		obj_to_cull[i]->SetActive(true);

}

void ComponentCamera::CameraCullingDynObj(GameObject* go) {

	if (!go->camera) {
		for (std::vector<GameObject*>::const_iterator it = go->childs.begin(); it < go->childs.end(); it++) {

			AABB refBox = (*it)->bounding_box;

			if (!(*it)->IsStatic() && refBox.IsFinite() && (*it)->mesh && (*it)->mesh->m_info.num_vertex > 0) {

				
				//if (!frustum.Intersects(refBox)) {    //MathGeoLib func. Slow but works better
				if (ContainsAABB(refBox) == OUTSIDE) {
				
					(*it)->SetActive(false);
				}
				else
					(*it)->SetActive(true);

			}

			CameraCullingDynObj(*it);

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

float* ComponentCamera::GetViewMatrix()
{
	view_mat = frustum.ViewMatrix();
	view_mat.Transpose();

	return (float*)view_mat.v;
}

float* ComponentCamera::GetProjectionMatrix()
{
	proj_mat = frustum.ProjectionMatrix();
	proj_mat.Transpose();

	return (float*)proj_mat.v;
}



void ComponentCamera::Save(JSON_Array* comp_array) {

	JSON_Value* value = json_value_init_object();
	JSON_Object* obj = json_value_get_object(value);

	json_object_set_number(obj, "Component Type", type);
	json_object_set_number(obj, "Aspect Ratio", aspect_ratio);
	json_object_set_number(obj, "FOV", GetFOV());
	json_object_set_number(obj, "Far Plane", frustum.farPlaneDistance);
	json_object_set_number(obj, "Near Plane", frustum.nearPlaneDistance);

	json_object_set_boolean(obj, "culling", culling);
	
	json_array_append_value(comp_array, value);
}


void ComponentCamera::Load(JSON_Object* comp_obj) {

	SetAspectRatio(json_object_get_number(comp_obj, "Aspect Ratio"));
	SetFOV(json_object_get_number(comp_obj, "FOV"));
	SetFarPlane(json_object_get_number(comp_obj, "Far Plane"));
	SetNearPlane(json_object_get_number(comp_obj, "Near Plane"));
	culling = json_object_get_boolean(comp_obj, "culling");

}