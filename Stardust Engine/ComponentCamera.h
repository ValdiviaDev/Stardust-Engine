#ifndef __ComponentCamera__H__
#define __ComponentCamera__H__


#include "Globals.h"
#include "Component.h"
#include "MathGeoLib/include/Geometry/Frustum.h"
#include "MathGeoLib/include/Geometry/AABB.h"
#include "MathGeoLib/include/Math/float4x4.h"

class GameObject;

enum Intersection {
	INSIDE = 0,
	INTERSECT = 1,
	OUTSIDE = 2
};

class ComponentCamera : public Component
{
public:
	ComponentCamera(GameObject* game_object = nullptr);
	~ComponentCamera();
	
	void Update();

	void Save(JSON_Array* comp_array) const;
	void Load(JSON_Object* comp_obj);

	void DrawInspector();

	float GetFOV() const;
	void SetFOV(float fov);
	void SetAspectRatio(float AR);
	void SetNearPlane(float near_plane);
	float GetNearPlane() const;
	void SetFarPlane(float far_plane);
	float GetFarPlane() const;

	void DrawFrustum();

	void CameraCullingStObj();
	void CameraCullingDynObj(GameObject* go);
	int ContainsAABB(const AABB& refBox) const;

	//View matrix and Projection matrix
	float* GetViewMatrix();
	float* GetProjectionMatrix();

public:
	math::Frustum frustum;
	bool main_camera = false;

private:
	float4x4 view_mat;
	float4x4 proj_mat;
	float aspect_ratio = 0.0f;
	bool culling = false;

};



#endif