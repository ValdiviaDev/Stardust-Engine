#include "ComponentMesh.h"


ComponentMesh::ComponentMesh(GameObject* parent) : Component(parent)
{
	type = Comp_Mesh;
}


ComponentMesh::~ComponentMesh()
{
}
