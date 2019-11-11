#ifndef __Component__H__
#define __Component__H__

#include "Parson/parson.h"

class GameObject;
struct json_object_t;
typedef struct json_object_t JSON_Object;

struct json_value_t;
typedef struct json_value_t  JSON_Value;

struct json_array_t;
typedef struct json_array_t  JSON_Array;

enum ComponentType {
	Comp_Default,
	Comp_Transform,
	Comp_Mesh,
	Comp_Material,
	Comp_Camera
};

class Component
{
public:
	Component(GameObject* parent);
	virtual ~Component();

	virtual void Enable(){} //TODO
	virtual void Update(){} //TODO
	virtual void Disable(){} //TODO
	virtual void Update(float dt) {}
	virtual void  DrawInspector() {}
	virtual void Save(JSON_Array* go_array) {}
	virtual void Load(JSON_Object* comp_obj) {}

	bool IsActive() const;
	void SetActive(bool active);

	GameObject* GetGameObject() const;

protected:
	ComponentType type = Comp_Default;
	bool active = true;
	GameObject* gameObject = nullptr;
};

#endif