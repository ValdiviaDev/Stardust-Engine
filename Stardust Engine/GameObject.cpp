#include "GameObject.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"

GameObject::GameObject(GameObject* parent)
{
	this->parent = parent;

	if (parent != nullptr) {
		parent->childs.push_back(this);
	}
	name = "Default GO name";
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

Component* GameObject::CreateComponent(ComponentType type, const char* path)
{
	Component* component = nullptr;

	switch (type) {
	case Comp_Transform:
		transform = new ComponentTransform(this);
		component = transform;
		break;
	case Comp_Mesh:
		mesh = new ComponentMesh(this, path);
		component = mesh;
		break;
	case Comp_Material:
		material = new ComponentMaterial(this);
		component = material;
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


uint GameObject::GetNumChilds() const
{
	return childs.size();
}

GameObject* GameObject::GetAChild(uint i) 
{
	return childs[i];

}

GameObject* GameObject::GetParent() {
	return parent;
}