#include "Component.h"
#include "GameObject.h"

Component::Component(GameObject* parent) : gameObject(parent)
{
}

Component::~Component()
{
}

bool Component::IsActive() const
{
	return active;
}

void Component::SetActive(bool active)
{
	this->active = active;
}

ComponentType Component::GetType() const
{
	return type;
}

GameObject* Component::GetGameObject() const
{
	return gameObject;
}

