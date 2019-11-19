#ifndef __ComponentMesh__H__
#define __ComponentMesh__H__

#include "Component.h"
#include "Application.h"

struct geo_info {

	//Vertex info
	uint id_vertex = 0;
	uint num_vertex = 0;
	float* vertex = nullptr;

	//Index info
	uint id_index = 0;
	uint num_index = 0;
	uint* index = nullptr;

	//Normals info
	uint id_normal = 0;
	uint num_normal = 0;
	float* normal = nullptr;

	//UVs info
	uint id_uv = 0;
	uint num_uv = 0;
	float* uv = nullptr;

	bool has_no_triangle = false;

};

class ComponentMesh : public Component //TODO
{
public:
	ComponentMesh(GameObject* parent, PrimitiveType primitive = PRIMITIVE_NONE);
	~ComponentMesh();

	void Save(JSON_Array* comp_array);
	void Load(JSON_Object* comp_obj);

	geo_info GetInfo() const;
	bool IsPrimitive() const;

	void  DrawInspector();

	void FillPrimitiveDrawInfo(par_shapes_mesh* info);
	bool LoadMesh(string name);

	void SetPath(const char* path);

	void BindBuffers();
	void BindBuffersPrimitive();
	

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

