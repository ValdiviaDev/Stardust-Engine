#include "ComponentMesh.h"
#include "Globals.h"

ComponentMesh::ComponentMesh(GameObject* parent, char* path, int num_mesh) : Component(parent), path(path), num_mesh(num_mesh)
{
	type = Comp_Mesh;

	AssignMesh(path);
}

ComponentMesh::~ComponentMesh()
{
	RELEASE_ARRAY(m_info.index);
	RELEASE_ARRAY(m_info.vertex);
	RELEASE_ARRAY(m_info.normal);
	RELEASE_ARRAY(m_info.uv);
	RELEASE_ARRAY(m_info.color);
}

void ComponentMesh::AssignMesh(char* path)
{
	//Path
	//char* path_source = "Assets/Meshes/";
	//char full_path[100];
	//strcpy(full_path, path_source);
	//strcat(full_path, path);

	bool charged = false;

	//Import mesh and bind buffers
	charged = App->importer->ImportMesh(path, m_info, this->gameObject, num_mesh);
	if (charged) {
		App->importer->BindBuffers(m_info);
		this->path = path;
	}
}

geo_info ComponentMesh::GetInfo() const
{
	return m_info;
}
