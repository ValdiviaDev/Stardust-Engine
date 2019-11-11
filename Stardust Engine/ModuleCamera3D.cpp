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
	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(0.0f, 0.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);

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

		//Lesft and right
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) 
			newPos -= dummy_cam->frustum.WorldRight() *  keys_speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) 
			newPos += dummy_cam->frustum.WorldRight() * keys_speed;

		//Position += newPos;
		//Reference += newPos;
		Move(newPos);

	}
	
	// Mouse wheel ----------------
	if (App->input->GetMouseZ()) {
		newPos += dummy_cam->frustum.front * wheel_speed * App->input->GetMouseZ();
		Move(newPos);
	}
	
	//Position += newPos;
	//Reference += newPos;

	// Mouse motion ----------------

	if(App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.25f;

		//Position -= Reference;

		if(dx != 0)
		{
			//Rotate arround the Y axis to rotate in the X coord axis
			Quat quat = Quat::RotateY(dx*dt*Sensitivity);
			dummy_cam->frustum.up = quat.Mul(dummy_cam->frustum.up).Normalized();
			dummy_cam->frustum.front = quat.Mul(dummy_cam->frustum.front).Normalized();
			//float DeltaX = (float)dx * Sensitivity;
			//
			//X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			//Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			//Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
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
			
			//float DeltaY = (float)dy * Sensitivity;
			//
			//Y = rotate(Y, DeltaY, X);
			//Z = rotate(Z, DeltaY, X);
			//
			//if(Y.y < 0.0f)
			//{
			//	Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
			//	Y = cross(Z, X);
			//}
		}

		//Position = Reference + Z * length(Position);
	}


	//Alt+LeftClick

	// Mouse motion ----------------

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.35f;

		//Reference is the GameObject we are pointing to

		GameObject* focus = App->scene->GetFocusedGameObject(App->scene->GetRootGameObject());
		if (focus && focus->transform) {
			float3 GO_pos = focus->transform->GetGlobalPos();
			LookAt(GO_pos);
		}
		else
			LookAt(float3::zero);



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

	}


	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		GameObject* focus = App->scene->GetFocusedGameObject(App->scene->GetRootGameObject());
		if (focus != nullptr && focus->mesh != nullptr)
			focus->CenterCameraOnGO();
	}

	// Recalculate matrix -------------
	//CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const float3 &Position, const float3 &Reference, bool RotateAroundReference)
{
	//Old
	//this->Position = Position;
	//this->Reference = Reference;
	//
	//Z = normalize(Position - Reference);
	//X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	//Y = cross(Z, X);
	//
	//if(!RotateAroundReference)
	//{
	//	this->Reference = this->Position;
	//	this->Position += Z * 0.05f;
	//}
	//
	//CalculateViewMatrix();
	
	//New
	dummy_cam->frustum.pos = Position;
	LookAt(Reference);
	
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const float3 &Spot)
{
	//Old
	//Reference = Spot;
	//
	//Z = normalize(Position - Reference);
	//X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	//Y = cross(Z, X);
	//
	//CalculateViewMatrix();

	//New TODO
	float3 dir = Spot - dummy_cam->frustum.pos;
	dir.Normalize();
	float4x4 mat = float4x4::LookAt(dummy_cam->frustum.front, dir, dummy_cam->frustum.up, float3::unitY);
	dummy_cam->frustum.front = mat.MulDir(dummy_cam->frustum.front).Normalized();
	dummy_cam->frustum.up = mat.MulDir(dummy_cam->frustum.up).Normalized();
	
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const float3 &Movement)
{
	//Old
	//Position += Movement;
	//Reference += Movement;
	//
	//CalculateViewMatrix();

	//New TODO
	dummy_cam->frustum.Translate(Movement);
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}


