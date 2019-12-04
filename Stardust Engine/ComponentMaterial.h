#ifndef __ComponentMaterial__H__
#define __ComponentMaterial__H__

#include "Component.h"
#include "Globals.h"


class ComponentMaterial : public Component //TODO
{
public:
	ComponentMaterial(GameObject* parent);
	~ComponentMaterial();

	void Save(JSON_Array* comp_array) const;
	void Load(JSON_Object* comp_obj);

	bool HasTex() const;

	void SetPath(const char* path);

	void DrawInspector();

public:
	UID uuid_mat = 0;

	//Debug
	bool debug_checkers = false;

private:

	char tex_path[500];
};

#endif