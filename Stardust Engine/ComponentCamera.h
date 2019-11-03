#ifndef __ComponentMesh__H__
#define __ComponentMesh__H__


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


private:

	


};



#endif