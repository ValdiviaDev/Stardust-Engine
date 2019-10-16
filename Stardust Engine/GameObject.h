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
	GameObject(GameObject* parent);
	virtual ~GameObject();

	void Update(); //TODO
	Component* CreateComponent(ComponentType type, char* path = "", int num_mesh = 0, bool is_primitive = false);

	bool IsActive() const;
	void SetActive(bool active);

	uint GetNumChilds() const;
	GameObject* GetChild(uint i);
	GameObject* GetParent();
	void DeleteFromParentList();

	void SetName(const char* new_name);
	const char* GetName()const;

	void GUIHierarchyPrint(int& i, bool& clicked);//Send false on clicked bool

	void DrawComponentsInspector();

	void CenterCameraOnGO() const;

private:
	bool active = true;
	const char* name;
	GameObject* parent = nullptr;
	

public:
	//There can be only one component of each type
	ComponentTransform* transform = nullptr;
	ComponentMesh* mesh = nullptr;
	ComponentMaterial* material = nullptr;

	std::vector<GameObject*> childs;
	bool focused = false;
};

#endif