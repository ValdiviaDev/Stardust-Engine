#ifndef __ComponentMesh__H__
#define __ComponentMesh__H__

#include "Component.h"
#include "Application.h"
#include "ModuleImport.h"

class ComponentMesh : public Component //TODO
{
public:
	ComponentMesh(GameObject* parent, char* path, int num_mesh);
	~ComponentMesh();

	void AssignMesh(char* path);

	geo_info GetInfo() const;

private:
	geo_info m_info;
	const char* path = nullptr;
	int num_mesh = 0; //Ambiguous name and maybe not necessary TODO
};

#endif

