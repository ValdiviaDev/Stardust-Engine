#include "ComponentMesh.h"
#include "Globals.h"

ComponentMesh::ComponentMesh(GameObject* parent, const char* path) : Component(parent), path(path)
{
	type = Comp_Mesh;

	//Path
	char* path_source = "Assets/Meshes/";
	char full_path[100];
	strcpy(full_path, path_source);
	strcat(full_path, path);

	//Import mesh and bind buffers
	App->importer->ImportMesh((char*)full_path, m_info);
	App->importer->BindBuffers(m_info);
}

ComponentMesh::~ComponentMesh()
{
	RELEASE_ARRAY(m_info.index);
	RELEASE_ARRAY(m_info.vertex);
	RELEASE_ARRAY(m_info.normal);
	RELEASE_ARRAY(m_info.uv);
	RELEASE_ARRAY(m_info.color);
}

geo_info ComponentMesh::GetInfo() const
{
	return m_info;
}
