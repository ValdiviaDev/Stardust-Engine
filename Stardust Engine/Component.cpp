#include "Component.h"
#include "GameObject.h"

Component::Component(GameObject* parent) : p_GameObject(parent)
{
}

Component::~Component()
{
}

bool Component::GetActive() const
{
	return active;
}

void Component::SetActive(bool active)
{
	this->active = active;
}

GameObject* Component::GetParentGO() const
{
	return p_GameObject;
}

