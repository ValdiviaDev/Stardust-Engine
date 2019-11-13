#ifndef __ModuleCamera3D_H__
#define __ModuleCamera3D_H__

#include "Module.h"
#include "Globals.h"

class GameObject;
class ComponentCamera;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	//Moving camera
	void Look(const float3 &Position, const float3 &Reference);
	void LookAt(const float3 &Spot);
	void Move(const float3 &Movement);

	//Mouse picking
	void CheckForMousePicking();
	void TestAABBPicking(LineSegment ray, GameObject* inters_GO, std::vector<GameObject*>& intersected_objs, float& min_dist, GameObject*& nearest);
	bool GetTrianglePicking(GameObject* object, LineSegment ray);

public:

	ComponentCamera* dummy_cam = nullptr;

};

#endif