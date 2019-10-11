#ifndef __ComponentMaterial__H__
#define __ComponentMaterial__H__

#include "Component.h"

class ComponentMaterial : public Component //TODO
{
public:
	ComponentMaterial(GameObject* parent);
	~ComponentMaterial();
};

#endif