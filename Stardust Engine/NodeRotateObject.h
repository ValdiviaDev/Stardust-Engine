#ifndef __NodeRotateObject_H__
#define __NodeRotateObject_H__

#include "Node.h"
#include "MathGeoLib/include/MathGeoLib.h"

class NodeRotateObject : public Node
{
public:
	NodeRotateObject(int id, const ImVec2& pos);
	~NodeRotateObject();

	bool Update(float dt, std::vector<GameObject*> BB_objects);
	void Draw(std::vector<GameObject*> BB_objects);

private:

	math::float3 rot_vel;
	bool rot_with_mouse;
	int rot_axis[3];

};

#endif