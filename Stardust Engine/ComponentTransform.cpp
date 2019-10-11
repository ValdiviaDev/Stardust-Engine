#include "ComponentTransform.h"


ComponentTransform::ComponentTransform(GameObject* parent) : Component(parent)
{
	type = Comp_Transform;
}


ComponentTransform::~ComponentTransform()
{
}
