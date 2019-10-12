#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleImport.h"


ComponentMaterial::ComponentMaterial(GameObject* parent) : Component(parent)
{
	type = Comp_Material;
}


ComponentMaterial::~ComponentMaterial()
{
}

void ComponentMaterial::AssignTexture(const char * path)
{
	//Path
	char* path_source = "Assets/Textures/";
	char full_path[100];
	strcpy(full_path, path_source);
	strcat(full_path, path);

	//Import texture with DevIL
	bool charged = App->importer->ImportTexture((char*)full_path, tex_id);

	if (charged) {
		has_tex = true;
		this->tex_path = full_path;
	}
}

uint ComponentMaterial::GetTexId() const
{
	return tex_id;
}

bool ComponentMaterial::GetIfTex() const
{
	return has_tex;
}
