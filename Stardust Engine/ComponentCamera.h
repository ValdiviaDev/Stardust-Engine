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

	void Save(JSON_Array* comp_array);

	void DrawInspector();

	float GetFOV() const;
	void SetFOV(float fov);
	void SetNearPlane(float near_plane);
	float GetNearPlane() const;
	void SetFarPlane(float far_plane);
	float GetFarPlane() const;

	void DrawFrustum();

	void CameraCulling(GameObject* go); //send root 1st time
	int ContainsAABB(const AABB& refBox) const;

	//View matrix and Projection matrix
	float* GetViewMatrix();
	float* GetProjectionMatrix();

public:
	math::Frustum frustum;

private:
	float4x4 view_mat;
	float4x4 proj_mat;
	float aspect_ratio = 0.0f;
	bool culling = false;

};



#endif