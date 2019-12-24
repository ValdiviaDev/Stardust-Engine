#ifndef __NodeInstantiateObject_H__
#define __NodeInstantiateObject_H__

#include "Node.h"
#include "MathGeoLib/include/MathGeoLib.h"

class ComponentMesh;
class ComponentMaterial;
class ComponentCamera; 
class ComponentGraphScript;

class NodeInstantiateObject : public Node
{
public:
	NodeInstantiateObject(int id, const ImVec2& pos);
	~NodeInstantiateObject();
	
	bool Update(float dt, std::vector<GameObject*> BB_objects);
	void Draw(std::vector<GameObject*> BB_objects);

	void InstanceObject(GameObject* to_instance);
	void CopyMesh(GameObject* new_inst, ComponentMesh* to_instance);
	void CopyMaterial(GameObject* new_inst, ComponentMaterial* to_instance);
	void CopyCamera(GameObject* new_inst, ComponentCamera* to_instance);
	void CopyCompGraph(GameObject* new_inst, ComponentGraphScript* to_instance);

	void ObjectToInstanceDropDown(std::vector<GameObject*> BB_objects);

	void Save(JSON_Object* obj) const;
	void Load(JSON_Object* obj);

private:
	bool get_ref_trans = false;

	math::float3 pos_to_inst = math::float3::zero;
	math::float3 rot_to_inst = math::float3::zero;

	int inst_indx = 0;

};

#endif