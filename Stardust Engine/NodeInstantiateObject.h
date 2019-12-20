#ifndef __NodeInstantiateObject_H__
#define __NodeInstantiateObject_H__

#include "Node.h"
#include "MathGeoLib/include/MathGeoLib.h"

class ComponentMesh;

class NodeInstantiateObject : public Node
{
public:
	NodeInstantiateObject(int id, const ImVec2& pos);
	~NodeInstantiateObject();
	
	bool Update(float dt, std::vector<GameObject*> BB_objects);
	void Draw(std::vector<GameObject*> BB_objects);

	void CopyMesh(GameObject* new_inst, ComponentMesh* to_instance);

	void ObjectToInstanceDropDown(std::vector<GameObject*> BB_objects);

private:
	bool get_ref_trans = false;

	math::float3 pos_to_inst = math::float3::zero;
	math::float3 rot_to_inst = math::float3::zero;

	int inst_indx = 0;

};

#endif