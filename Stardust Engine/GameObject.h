#ifndef __GameObject__H__
#define __GameObject__H__

#include <vector>
class Component;
enum ComponentType;

class GameObject
{
public:
	GameObject();  //TODO
	~GameObject();

	void Update(); //TODO
	Component* CreateComponent(ComponentType type);

	bool GetActive() const;
	void SetActive(bool active);

private:
	bool active = true;
	const char* name;
	std::vector<Component*> components;
};

#endif