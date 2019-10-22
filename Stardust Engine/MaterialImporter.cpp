#include "MaterialImporter.h"
#include "ComponentMaterial.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")
#pragma comment (lib, "DevIL/libx86/DevIL.lib")
#pragma comment (lib, "DevIL/libx86/ILU.lib")
#pragma comment (lib, "DevIL/libx86/ILUT.lib")
#include "DevIL/include/il.h"
#include "DevIL/include/ilu.h"
#include "DevIL/include/ilut.h"

MaterialImporter::MaterialImporter()
{
}


MaterialImporter::~MaterialImporter()
{
}

bool MaterialImporter::Import(const char * file, const char * path, std::string & output_file)
{
	return false;
}

bool MaterialImporter::Import(const void * buffer, uint size, std::string & output_file)
{
	return false;
}

bool MaterialImporter::Load(const char * exported_file, ComponentMaterial * resource)
{
	return false;
}

bool MaterialImporter::LoadCheckers(ComponentMaterial * resource)
{
	return false;
}
