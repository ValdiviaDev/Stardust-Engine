#ifndef __Component__H__
#define __Component__H__

class GameObject;

enum ComponentType {
	Comp_Default,
	Comp_Transform,
	Comp_Mesh,
	Comp_Material
};

class Component
{
public:
	Component(GameObject* parent);
	virtual ~Component();

	virtual void Enable(){} //TODO
	virtual void Update(){} //TODO
	virtual void Disable(){} //TODO

	bool IsActive() const;
	void SetActive(bool active);

	GameObject* GetGameObject() const;

protected:
	ComponentType type = Comp_Default;
	bool active = true;
	GameObject* gameObject;
};

#endif