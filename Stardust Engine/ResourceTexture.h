#ifndef __ResourceTexture_H__
#define __ResourceTexture_H__

#include "Resource.h"

class ResourceTexture : public Resource
{
public:
	ResourceTexture(UID uuid);
	~ResourceTexture();

	bool LoadInMemory();
	bool EraseInMemory();

private:
	uint tex_id = 0;
	int tex_width, tex_height;

};

#endif