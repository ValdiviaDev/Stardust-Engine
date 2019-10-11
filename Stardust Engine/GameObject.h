#ifndef __GameObject__H__
#define __GameObject__H__

#include "Globals.h"
#include "ComponentTransform.h"
#include <vector>
class Component;
enum ComponentType;

class GameObject
{
public:
	//GameObject();
	GameObject(GameObject* parent);
	~GameObject();

	void Update(); //TODO
	Component* CreateComponent(ComponentType type);

	bool GetActive() const;
	void SetActive(bool active);

	uint GetNumChilds() const;
	GameObject* GetAChild(uint i);
	GameObject* GetParent();


private:
	bool active = true;
	const char* name;
	std::vector<Component*> components;
	GameObject* parent = nullptr;
	std::vector<GameObject*> childs;

public:
	ComponentTransform* transform = nullptr;
};

#endif