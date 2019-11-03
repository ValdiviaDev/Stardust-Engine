#ifndef __ComponentCamera__H__
#define __ComponentCamera__H__


#include "Globals.h"
#include "Component.h"
#include "MathGeoLib/include/Geometry/Frustum.h"
#include "MathGeoLib/include/Geometry/AABB.h"

class GameObject;

class ComponentCamera : public Component
{
public:
	ComponentCamera(GameObject* game_object = nullptr);
	~ComponentCamera();
	
	void Update();

	void DrawInspector();

	float GetFOV() const;
	void SetFOV(float fov);
	void SetNearPlane(float near_plane);
	float GetNearPlane() const;
	void SetFarPlane(float far_plane);
	float GetFarPlane() const;

	void DrawFrustum();

private:

	math::Frustum frustum;
	float aspect_ratio = 0.0f;


};



#endif