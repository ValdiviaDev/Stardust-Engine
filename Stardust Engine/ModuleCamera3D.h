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

	//User camera moving
	void MoveArroundEngine(float keys_speed, float dt);
	void OrbitArroundEngine(float dt);
	void OrbitArroundObject(float dt);
	void ZoomInOut(float wheel_speed);
	void FocusInObject();

	//Mouse picking
	void CheckForMousePicking();
	void DynObjAABBIntersect(LineSegment ray, GameObject* inters_GO, std::vector<GameObject*>& intersected_objs);
	void GetAABBClosestObject(LineSegment ray, std::vector<GameObject*> intersected_objs, GameObject*& nearest);
	bool TestTrianglePicking(LineSegment ray, std::vector<GameObject*> intersected_objs, GameObject*& nearest);

public:

	ComponentCamera* dummy_cam = nullptr;

};

#endif