#include "GameObject.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "Globals.h"


GameObject::GameObject()
{
}


GameObject::~GameObject()
{
	//Delete Components
	for (int i = 0; i < components.size(); ++i)
		RELEASE(components[i]);
}

void GameObject::Update()
{
}

Component* GameObject::CreateComponent(ComponentType type)
{
	Component* component = nullptr;

	switch (type) {
	case Comp_Transform:
		component = new ComponentTransform(this);
		break;
	case Comp_Mesh:
		component = new ComponentMesh(this);
		break;
	case Comp_Material:
		component = new ComponentMaterial(this);
		break;
	case Comp_Default:
		component = new Component(this);
		break;
	}
	components.push_back(component);

	return component;
}

bool GameObject::GetActive() const
{
	return active;
}

void GameObject::SetActive(bool active)
{
	this->active = active;
}
