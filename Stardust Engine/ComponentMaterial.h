#ifndef __ComponentMaterial__H__
#define __ComponentMaterial__H__

#include "Component.h"
#include "Globals.h"

class ComponentMaterial : public Component //TODO
{
public:
	ComponentMaterial(GameObject* parent);
	~ComponentMaterial();

	void Save(JSON_Array* comp_array);

	void AssignTexture(const char* path);

	uint GetTexId()const;
	bool GetIfTex()const;

	void  DrawInspector();

public:
	bool debug_tex_draw = true;
	bool debug_checkers = false;

	uint tex_id = 0;
	int tex_width, tex_height;

private:
	const char* tex_path;
	bool has_tex = false;
};

#endif