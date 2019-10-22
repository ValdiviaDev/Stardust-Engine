#include "MeshImporter.h"
#include "ComponentMesh.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"


MeshImporter::MeshImporter()
{
}


MeshImporter::~MeshImporter()
{
}

bool MeshImporter::Import(const char * file, const char * path, std::string & output_file)
{
	return false;
}

bool MeshImporter::Import(const void * buffer, uint size, std::string & output_file)
{
	return false;
}

bool MeshImporter::Load(const char * exported_file, ComponentMesh * resource)
{
	return false;
}
