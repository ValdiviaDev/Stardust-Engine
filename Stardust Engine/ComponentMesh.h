#ifndef __ComponentMesh__H__
#define __ComponentMesh__H__

#include "Component.h"

class ComponentMesh : public Component
{
public:
	ComponentMesh(GameObject* parent);
	~ComponentMesh();

	void Save(JSON_Array* comp_array) const;
	void Load(JSON_Object* comp_obj);

	void DrawInspector();
	void SetPath(const char* path);
	
	bool IsPrimitive() const;
	void SetPrimitive(PrimitiveType type);

public:
	uint uuid_mesh = 0;

	//Debug
	bool debug_v_norm = false;
	bool debug_f_norm = false;

private:
	const char* path = nullptr;
	PrimitiveType is_primitive = PRIMITIVE_NONE;

};

#endif

