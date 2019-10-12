#ifndef __ComponentMaterial__H__
#define __ComponentMaterial__H__

#include "Component.h"
#include "Globals.h"

class ComponentMaterial : public Component //TODO
{
public:
	ComponentMaterial(GameObject* parent);
	~ComponentMaterial();

	void AssignTexture(const char* path);

	uint GetTexId()const;
	bool GetIfTex()const;

private:
	const char* tex_path;
	bool has_tex = false;
	uint tex_id = 0;

};

#endif