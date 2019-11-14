#ifndef __ComponentMesh__H__
#define __ComponentMesh__H__

#include "Component.h"
#include "Application.h"
#include "ModuleImport.h"




class ComponentMesh : public Component //TODO
{
public:
	ComponentMesh(GameObject* parent, const char* path, PrimitiveType primitive = PRIMITIVE_NONE);
	~ComponentMesh();

	void Save(JSON_Array* comp_array);
	void Load(JSON_Object* comp_obj);

	void AssignMesh(const char* path);

	geo_info GetInfo() const;
	bool IsPrimitive() const;

	void  DrawInspector();

	void FillPrimitiveDrawInfo(par_shapes_mesh* info);
	bool LoadMesh(string name);

	void SetPath(const char* path);

	

public:
	bool debug_v_norm = false;
	bool debug_f_norm = false;
	geo_info m_info;
	uint uuid_mesh = 0;

private:
	const char* path = nullptr;
	PrimitiveType is_primitive = PRIMITIVE_NONE;
};

#endif

