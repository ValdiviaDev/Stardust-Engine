#ifndef __NodeRotateObject_H__
#define __NodeRotateObject_H__

#include "Node.h"
#include "MathGeoLib/include/MathGeoLib.h"

class NodeRotateObject : public Node
{
public:
	NodeRotateObject(int id, const ImVec2& pos);
	~NodeRotateObject();

	bool Update(float dt, std::vector<GameObject*> BB_objects, uint num_comp_graph);
	void Draw(std::vector<GameObject*> BB_objects);

	void Save(JSON_Object* obj) const;
	void Load(JSON_Object* obj);

private:

	math::float3 rot_vel;
	bool rot_with_mouse;

	bool mouse_rot_X = true;
	bool mouse_rot_Y = false;
	bool reset_rot = false;

};

#endif