#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleGui.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentCamera.h"
#include "MathGeoLib/include/Math/MathAll.h"
#include "Quadtree.h"
#include "ResourceMesh.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, "Camera3D", start_enabled)
{
	//Dummy cam
	engine_cam = new ComponentCamera();
	engine_cam->SetNearPlane(0.6f);
	engine_cam->SetFarPlane(512.0f);
	current_cam = engine_cam;
	Move({ 25.0f, 25.0f, 25.0f });
	LookAt({ 0.0f, 0.0f, 0.0f });
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	App->gui->AddLogToConsole("Setting up the camera");
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");
	RELEASE(engine_cam);

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	if (App->GetEngineState() == Engine_State_Editor) {
		// Mouse picking ----------------
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN) {
			if (!App->gui->IsMouseHoveringWindow()) //If any ImGui window is being pressed, don't check picking
				CheckForMousePicking();
		}

		// Keys motion and mouse motion ----------------
		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		{
			float keys_speed = 8.0f * dt;
			MoveArroundEngine(keys_speed, dt);

			OrbitArroundEngine(dt);
		}

		// Mouse wheel ----------------
		if (App->input->GetMouseZ() && !App->gui->IsMouseHoveringWindow()) {
			float wheel_speed = 1.0f /** dt*/;
			ZoomInOut(wheel_speed);
		}

		//Alt+LeftClick
		// Obrbit arround object ---------------- ????
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
			OrbitArroundObject(dt);

		//Focus on GameObject ----------------
		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
			FocusInObject();
	}

	return UPDATE_CONTINUE;
}

//Camera move ---------------------------------------------------------------------------

void ModuleCamera3D::Look(const float3 &Position, const float3 &Reference)
{
	
	current_cam->frustum.pos = Position;
	LookAt(Reference);
	
}

void ModuleCamera3D::LookAt(const float3 &Spot)
{
	float3 dir = Spot - current_cam->frustum.pos;
	dir.Normalize();
	float3x3 mat = float3x3::LookAt(current_cam->frustum.front, dir, current_cam->frustum.up, float3::unitY);
	current_cam->frustum.front = mat.MulDir(current_cam->frustum.front).Normalized();
	current_cam->frustum.up = mat.MulDir(current_cam->frustum.up).Normalized();
}

void ModuleCamera3D::Move(const float3 &Movement)
{
	current_cam->frustum.Translate(Movement);
}

//Camera move user ---------------------------------------------------------------------------

void ModuleCamera3D::MoveArroundEngine(float keys_speed, float dt)
{
	float3 newPos(0, 0, 0);

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		keys_speed *= 10.0f;

	//Up and down
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
		newPos.y += keys_speed;
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
		newPos.y -= keys_speed;

	//Forward and backwards
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		newPos += current_cam->frustum.front * keys_speed;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		newPos -= current_cam->frustum.front * keys_speed;

	//Left and right
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		newPos -= current_cam->frustum.WorldRight() *  keys_speed;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		newPos += current_cam->frustum.WorldRight() * keys_speed;

	Move(newPos);
}

void ModuleCamera3D::OrbitArroundEngine(float dt)
{
	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();

	float Sensitivity = 0.25f;

	if (dx != 0)
	{
		//Rotate arround the Y axis to rotate in the X coord axis
		Quat quat = Quat::RotateY(dx*dt*Sensitivity);
		current_cam->frustum.up = quat.Mul(current_cam->frustum.up).Normalized();
		current_cam->frustum.front = quat.Mul(current_cam->frustum.front).Normalized();
	}

	if (dy != 0)
	{
		//Rotate arround the X local axis to rotate in the Y coord axis
		Quat quat = Quat::RotateAxisAngle(current_cam->frustum.WorldRight(), dy*dt*Sensitivity);
		float3 up = quat.Mul(current_cam->frustum.up).Normalized();
		//Cap that you can be upside down in engine
		if (up.y > 0.0f) {
			current_cam->frustum.up = up;
			current_cam->frustum.front = quat.Mul(current_cam->frustum.front).Normalized();
		}
	}
}

void ModuleCamera3D::OrbitArroundObject(float dt)
{
	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();

	float Sensitivity = 0.35f;

	//Reference is the GameObject we are pointing to

	float3 point;
	GameObject* focus = App->scene->GetFocusedGameObject();
	if (focus && focus->transform) {
		float3 GO_pos = focus->transform->GetGlobalPos();
		point = GO_pos;
	}
	else
		point = float3::zero;

	if (dx != 0)
	{
		//Rotate arround the Y axis to rotate in the X coord axis
		Quat quat = Quat::RotateY(dx*dt*Sensitivity);
		current_cam->frustum.up = quat.Mul(current_cam->frustum.up).Normalized();
		current_cam->frustum.front = quat.Mul(current_cam->frustum.front).Normalized();
	}

	if (dy != 0)
	{
		//Rotate arround the X local axis to rotate in the Y coord axis
		Quat quat = Quat::RotateAxisAngle(current_cam->frustum.WorldRight(), dy*dt*Sensitivity);
		float3 up = quat.Mul(current_cam->frustum.up).Normalized();
		//Cap that you can be upside down in engine
		if (up.y > 0.0f) {
			current_cam->frustum.up = up;
			current_cam->frustum.front = quat.Mul(current_cam->frustum.front).Normalized();
		}

	}

	LookAt(point);
}

void ModuleCamera3D::ZoomInOut(float wheel_speed)
{
	float3 newPos(0, 0, 0);

	newPos += current_cam->frustum.front * wheel_speed * App->input->GetMouseZ();
	Move(newPos);
}

void ModuleCamera3D::FocusInObject()
{
	GameObject* focus = App->scene->GetFocusedGameObject();
	if (focus != nullptr && focus->mesh != nullptr)
		focus->CenterCameraOnGO();
}

//Mouse picking ---------------------------------------------------------------------------

void ModuleCamera3D::CheckForMousePicking()
{
	vector<GameObject*> intersected_objs;

	//Get the screen coords and normalize them
	int mouse_pos_x = App->input->GetMouseX();
	int mouse_pos_y = App->input->GetMouseY();
	int scr_width, scr_height;
	App->window->GetWinSize(scr_width, scr_height);

	//Normalize between -1 and 1
	float norm_x = -(1.0f - 2.0f * ((float)mouse_pos_x) / ((float)scr_width));
	float norm_y = 1.0f - (2.0f * ((float)mouse_pos_y) / ((float)scr_height));

	//Ray that goes from near plane to far plane (a near plane, b far plane)
	LineSegment picking = current_cam->frustum.UnProjectLineSegment(norm_x, norm_y);
	
	//Quadtree picking static intersected objects only looking in the TreeNode of the ray
	App->scene->quadtree->Intersect(intersected_objs, picking);
	
	//Get the AABB intersection of the dynamic objects
	GameObject* root_obj = App->scene->GetRootGameObject();
	for(int i = 0; i < root_obj->GetNumChilds(); ++i)
		DynObjAABBIntersect(picking, root_obj->childs[i], intersected_objs);

	//We have the whole vector of intersections. Test which object is closer
	GameObject* nearest_GO = nullptr;
	GetAABBClosestObject(picking, intersected_objs, nearest_GO);

	if (nearest_GO) { //If there's something to pick
		if (nearest_GO->mesh->IsPrimitive()) //If the nearest GO AABB is a primitive automaticaly focus
			App->scene->FocusGameObject(nearest_GO, App->scene->GetRootGameObject());
		else 
		{
			//Test the triangles of every mesh that isn't a primitive
			if(TestTrianglePicking(picking, intersected_objs, nearest_GO))
				App->scene->FocusGameObject(nearest_GO, App->scene->GetRootGameObject());
			else
				App->scene->UnfocusGameObjects();
		}
	}
	else
		App->scene->UnfocusGameObjects();

}

void ModuleCamera3D::DynObjAABBIntersect(LineSegment ray, GameObject* inters_GO, vector<GameObject*>& intersected_objs)
{
	//Look for AABB intersections. Look for all the intersected object and the closest object to the ray
	if (inters_GO->bounding_box.IsFinite() && !inters_GO->IsStatic()) {
		if(ray.Intersects(inters_GO->bounding_box))
			intersected_objs.push_back(inters_GO);
	}

	for (int i = 0; i < inters_GO->GetNumChilds(); ++i)
		DynObjAABBIntersect(ray, inters_GO->childs[i], intersected_objs);

}

void ModuleCamera3D::GetAABBClosestObject(LineSegment ray, std::vector<GameObject*> intersected_objs, GameObject*& nearest)
{
	float min_dist = FLOAT_INF;

	for (int i = 0; i < intersected_objs.size(); ++i) {
		float hit_dist = intersected_objs[i]->bounding_box.Distance(ray.a);
		if (hit_dist < min_dist) {
			nearest = intersected_objs[i];
			min_dist = hit_dist;
		}
	}
}

bool ModuleCamera3D::TestTrianglePicking(LineSegment ray, vector<GameObject*> intersected_objs, GameObject*& nearest)
{
	bool ret = false;
	float min_hit_dist = FLOAT_INF;

	//Check all the intersected objects in triangle level to check which is the nearest
	for (int i = 0; i < intersected_objs.size(); ++i) {

		if (intersected_objs[i]->mesh->IsPrimitive()) //If we got a primitive don't check the triangles
			continue;

		//Pass ray to local coordinates
		LineSegment local_ray = ray;
		local_ray.Transform(intersected_objs[i]->transform->GetGlobalMatrix().Inverted());

		//Check every mesh triangle
		ResourceMesh* m = (ResourceMesh*)App->resources->Get(intersected_objs[i]->mesh->uuid_mesh);

		for (int j = 0; j < m->num_index; j += 3) {
			//Triangle points
			uint index_01 = m->index[j] * 3;
			uint index_02 = m->index[j + 1] * 3;
			uint index_03 = m->index[j + 2] * 3;

			float3 p1 = { m->vertex[index_01], m->vertex[index_01 + 1], m->vertex[index_01 + 2] };
			float3 p2 = { m->vertex[index_02], m->vertex[index_02 + 1], m->vertex[index_02 + 2] };
			float3 p3 = { m->vertex[index_03], m->vertex[index_03 + 1], m->vertex[index_03 + 2] };

			Triangle tri = { p1, p2, p3 };

			//Check triangle intersection
			float distance;
			float3 hit_point;
			if (local_ray.Intersects(tri, &distance, &hit_point)) {
				ret = true;
				if (distance < min_hit_dist) {
					nearest = intersected_objs[i];
					min_hit_dist = distance;
				}
			}
		}
	}
	return ret;
}


