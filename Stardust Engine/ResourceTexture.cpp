#include "Application.h"
#include "ResourceTexture.h"
#include "MaterialImporter.h"
#include <string>
#include "Glew/include/glew.h"


ResourceTexture::ResourceTexture(UID uuid) : Resource(uuid, Resource_Texture)
{
}


ResourceTexture::~ResourceTexture()
{
}

bool ResourceTexture::LoadInMemory()
{
	bool ret = App->mat_import->LoadMaterial(std::to_string(uuid).c_str(), this);

	return ret;
}

bool ResourceTexture::UnloadInMemory()
{
	glDeleteTextures(1, (GLuint*)&tex_id);

	tex_id = 0;
	tex_width = 0;
	tex_height = 0;

	return true;
}
