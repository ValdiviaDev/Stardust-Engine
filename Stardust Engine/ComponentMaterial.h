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
	void Load(JSON_Object* comp_obj);

	void AssignTexture(const char* path);
	void AssignTextureLib(const char* path);

	uint GetTexId()const;
	bool GetIfTex()const;

	void SetPath(const char* path);

	void  DrawInspector();

public:
	bool debug_tex_draw = true;
	bool debug_checkers = false;

	uint tex_id = 0;
	int tex_width, tex_height;
	
	uint uuid_mat = 0;
private:
	char tex_path[500];
	bool has_tex = false;
};

#endif