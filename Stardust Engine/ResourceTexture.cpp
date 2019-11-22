#include "ResourceTexture.h"



ResourceTexture::ResourceTexture(UID uuid) : Resource(uuid, Resource_Texture)
{
}


ResourceTexture::~ResourceTexture()
{
}

bool ResourceTexture::LoadInMemory()
{
	return false;
}

bool ResourceTexture::UnloadInMemory()
{
	return false;
}
