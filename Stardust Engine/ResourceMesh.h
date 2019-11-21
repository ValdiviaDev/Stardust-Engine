#ifndef __ResourceMesh_H__
#define __ResourceMesh_H__

#include "Resource.h"

class ResourceMesh : public Resource
{
public:
	ResourceMesh(UID uuid);
	~ResourceMesh();

	bool LoadInMemory();
	bool EraseInMemory();

private:
	PrimitiveType is_primitive = PRIMITIVE_NONE;

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

};

#endif