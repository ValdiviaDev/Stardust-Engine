#ifndef __GameObject__H__
#define __GameObject__H__

#include "Globals.h"
#include <vector>
class Component;
class ComponentTransform;
class ComponentMesh;
class ComponentMaterial;
enum ComponentType;

class GameObject
{
public:
	//GameObject();
	GameObject(GameObject* parent);
	~GameObject();

	void Update(); //TODO
	Component* CreateComponent(ComponentType type, const char* path = "", int num_mesh = 0);

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
	ComponentMesh* mesh = nullptr;
	ComponentMaterial* material = nullptr;
};

#endif