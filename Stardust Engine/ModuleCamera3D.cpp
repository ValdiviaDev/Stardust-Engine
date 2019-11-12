#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleGui.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentCamera.h"
#include "MathGeoLib/include/Math/MathAll.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, "Camera3D", start_enabled)
{
	//Dummy cam
	dummy_cam = new ComponentCamera();
	dummy_cam->SetNearPlane(0.6f);
	dummy_cam->SetFarPlane(512.0f);
	Move({ 5.0f, 5.0f, 5.0f });
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
	RELEASE(dummy_cam);

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	CheckForMousePicking();

	// Keys motion ----------------
	float3 newPos(0, 0, 0);
	float wheel_speed = 1.0f /** dt*/;

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		float keys_speed = 8.0f * dt;
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
			keys_speed = 20.0f * dt;

		//Up and down
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) 
			newPos.y += keys_speed;
		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) 
			newPos.y -= keys_speed;

		//Forward and backwards
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) 
			newPos += dummy_cam->frustum.front * keys_speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) 
			newPos -= dummy_cam->frustum.front * keys_speed;

		//Left and right
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) 
			newPos -= dummy_cam->frustum.WorldRight() *  keys_speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) 
			newPos += dummy_cam->frustum.WorldRight() * keys_speed;

		Move(newPos);

	}
	
	// Mouse wheel ----------------
	if (App->input->GetMouseZ()) {
		newPos += dummy_cam->frustum.front * wheel_speed * App->input->GetMouseZ();
		Move(newPos);
	}
	
	// Mouse motion ----------------

	if(App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.25f;

		if(dx != 0)
		{
			//Rotate arround the Y axis to rotate in the X coord axis
			Quat quat = Quat::RotateY(dx*dt*Sensitivity);
			dummy_cam->frustum.up = quat.Mul(dummy_cam->frustum.up).Normalized();
			dummy_cam->frustum.front = quat.Mul(dummy_cam->frustum.front).Normalized();
		}

		if(dy != 0)
		{
			//Rotate arround the X local axis to rotate in the Y coord axis
			Quat quat = Quat::RotateAxisAngle(dummy_cam->frustum.WorldRight(), dy*dt*Sensitivity);
			float3 up = quat.Mul(dummy_cam->frustum.up).Normalized();
			//Cap that you can be upside down in engine
			if (up.y > 0.0f) {
				dummy_cam->frustum.up = up;
				dummy_cam->frustum.front = quat.Mul(dummy_cam->frustum.front).Normalized();
			}
		}

	}


	//Alt+LeftClick

	// Mouse motion ---------------- ????

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.35f;

		//Reference is the GameObject we are pointing to

		float3 point;
		GameObject* focus = App->scene->GetFocusedGameObject(App->scene->GetRootGameObject());
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
			dummy_cam->frustum.up = quat.Mul(dummy_cam->frustum.up).Normalized();
			dummy_cam->frustum.front = quat.Mul(dummy_cam->frustum.front).Normalized();
		}
		
		if (dy != 0)
		{
			//Rotate arround the X local axis to rotate in the Y coord axis
			Quat quat = Quat::RotateAxisAngle(dummy_cam->frustum.WorldRight(), dy*dt*Sensitivity);
			float3 up = quat.Mul(dummy_cam->frustum.up).Normalized();
			//Cap that you can be upside down in engine
			if (up.y > 0.0f) {
				dummy_cam->frustum.up = up;
				dummy_cam->frustum.front = quat.Mul(dummy_cam->frustum.front).Normalized();
			}
		
		}

		LookAt(point);

	}


	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		GameObject* focus = App->scene->GetFocusedGameObject(App->scene->GetRootGameObject());
		if (focus != nullptr && focus->mesh != nullptr)
			focus->CenterCameraOnGO();
	}

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const float3 &Position, const float3 &Reference)
{
	
	dummy_cam->frustum.pos = Position;
	LookAt(Reference);
	
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const float3 &Spot)
{
	float3 dir = Spot - dummy_cam->frustum.pos;
	dir.Normalize();
	float3x3 mat = float3x3::LookAt(dummy_cam->frustum.front, dir, dummy_cam->frustum.up, float3::unitY);
	dummy_cam->frustum.front = mat.MulDir(dummy_cam->frustum.front).Normalized();
	dummy_cam->frustum.up = mat.MulDir(dummy_cam->frustum.up).Normalized();	
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const float3 &Movement)
{
	dummy_cam->frustum.Translate(Movement);
}

void ModuleCamera3D::CheckForMousePicking()
{
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		//Get the screen coords and normalize them
		int mouse_pos_x = App->input->GetMouseX();
		int mouse_pos_y = App->input->GetMouseY();
		int scr_width, scr_height;
		App->window->GetWinSize(scr_width, scr_height);

		//Normalize between -1 and 1
		float norm_x = -(1.0f - 2.0f * ((float)mouse_pos_x) / ((float)scr_width));
		float norm_y = 1.0f - (2.0f * ((float)mouse_pos_y) / ((float)scr_height));

		//Ray that goes from near plane to far plane
		LineSegment picking = dummy_cam->frustum.UnProjectLineSegment(norm_x, norm_y);
		
		//Check if the ray hits any AABB of the objects in scene and get the nearest object
		GameObject* selected_GO = GetNearestPickedGO(picking);

		//Check if the ray hits the mesh of the nearest object and check triangle by triangle
		bool hit = false;
		if(selected_GO)
			hit = GetTrianglePicking(selected_GO, picking);

		//Focus
		if (!App->gui->IsMouseHoveringWindow()) { //If any ImGui window is being pressed, don't interact with the scene
			if (selected_GO && hit)
				App->scene->FocusGameObject(selected_GO, App->scene->GetRootGameObject());
			else
				App->scene->UnfocusGameObjects();
		}

	}
}

GameObject * ModuleCamera3D::GetNearestPickedGO(LineSegment ray)
{
	float min_hit_dist = FLOAT_INF;
	GameObject* selected_GO = nullptr;

	for (int i = 0; i < App->scene->scene_GOs.size(); ++i) {
		if (App->scene->scene_GOs[i]->bounding_box.IsFinite()) {
			float hit_dist, hit_dist_far;
			bool hit = ray.Intersects(App->scene->scene_GOs[i]->bounding_box, hit_dist, hit_dist_far);
			if (hit) {
				if (hit_dist < min_hit_dist) {
					min_hit_dist = hit_dist;
					selected_GO = App->scene->scene_GOs[i];
				}
			}
		}
	}

	return selected_GO;
}

bool ModuleCamera3D::GetTrianglePicking(GameObject* object, LineSegment ray)
{
	bool ret = false;

	//TODO: fix primitives !!!!!!!!!
	if (object->mesh->IsPrimitive())
		return true;

	//Pass ray to local coordinates
	LineSegment local_ray = ray;
	local_ray.Transform(object->transform->GetGlobalMatrix().Inverted());

	//Check every mesh triangle
	geo_info m = object->mesh->GetInfo();

	for (int i = 0; i < m.num_index; i += 3) {
		//Triangle points
		uint index_01 = m.index[i] * 3;
		uint index_02 = m.index[i + 1] * 3;
		uint index_03 = m.index[i + 2] * 3;

		float3 p1 = { m.vertex[index_01], m.vertex[index_01 + 1], m.vertex[index_01 + 2] };
		float3 p2 = { m.vertex[index_02], m.vertex[index_02 + 1], m.vertex[index_02 + 2] };
		float3 p3 = { m.vertex[index_03], m.vertex[index_03 + 1], m.vertex[index_03 + 2] };

		Triangle tri = { p1, p2, p3 };

		//Check triangle intersection
		float distance;
		float3 hit_point;
		bool hit = local_ray.Intersects(tri, &distance, &hit_point);

		if (hit) //Todo: do stuff with distance
			ret = true;
	}

	return ret;
}


