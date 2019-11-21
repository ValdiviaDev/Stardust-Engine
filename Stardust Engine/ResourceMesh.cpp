#include "ResourceMesh.h"


ResourceMesh::ResourceMesh(UID uuid) : Resource(uuid, Resource_Mesh)
{
}

ResourceMesh::~ResourceMesh()
{
}

bool ResourceMesh::LoadInMemory()
{
	return false;
}

bool ResourceMesh::EraseInMemory()
{
	return false;
}
