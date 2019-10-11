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
	~Component();

	virtual void Enable(){} //TODO
	virtual void Update(){} //TODO
	virtual void Disable(){} //TODO

	bool GetActive() const;
	void SetActive(bool active);

	GameObject* GetParentGO() const;
	
protected:
	ComponentType type = Comp_Default;
	bool active = true;
	GameObject* p_GameObject;
};

#endif