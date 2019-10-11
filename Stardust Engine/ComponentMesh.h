#ifndef __ComponentMesh__H__
#define __ComponentMesh__H__

#include "Component.h"
#include "Application.h"
#include "ModuleImport.h"

class ComponentMesh : public Component //TODO
{
public:
	ComponentMesh(GameObject* parent, const char* path, int num_mesh);
	~ComponentMesh();

	geo_info GetInfo() const;

private:
	geo_info m_info;
	const char* path = nullptr;

};

#endif

