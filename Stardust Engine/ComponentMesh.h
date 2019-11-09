#ifndef __ComponentMesh__H__
#define __ComponentMesh__H__

#include "Component.h"
#include "Application.h"
#include "ModuleImport.h"

class ComponentMesh : public Component //TODO
{
public:
	ComponentMesh(GameObject* parent, char* path, bool is_primitive);
	~ComponentMesh();

	void Save(JSON_Array* comp_array);

	void AssignMesh(char* path);

	geo_info GetInfo() const;
	bool IsPrimitive() const;

	void  DrawInspector();

	void FillPrimitiveDrawInfo(par_shapes_mesh* info);
	void LoadMesh(string name);

	void SetPath(const char* path);

	

public:
	bool debug_v_norm = false;
	bool debug_f_norm = false;
	geo_info m_info;

private:
	const char* path = nullptr;
	int is_primitive = false;
};

#endif

