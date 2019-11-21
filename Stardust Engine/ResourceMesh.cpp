#include "Application.h"
#include "ResourceMesh.h"


ResourceMesh::ResourceMesh(UID uuid) : Resource(uuid, Resource_Mesh)
{
}

ResourceMesh::~ResourceMesh()
{
}

bool ResourceMesh::LoadInMemory()
{
	//geo_info m;
	//const char* f = (const char*)uuid;
	//App->mesh_import->LoadMesh(f, m);

	//Bind buffers

	return false;
}

bool ResourceMesh::EraseInMemory()
{
	return false;
}
