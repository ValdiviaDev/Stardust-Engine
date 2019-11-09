#ifndef __ComponentCamera__H__
#define __ComponentCamera__H__


#include "Globals.h"
#include "Component.h"
#include "MathGeoLib/include/Geometry/Frustum.h"
#include "MathGeoLib/include/Geometry/AABB.h"

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
private:

	math::Frustum frustum;
	float aspect_ratio = 0.0f;
	bool culling = false;

};



#endif