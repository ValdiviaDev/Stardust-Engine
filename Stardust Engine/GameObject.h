#ifndef __GameObject__H__
#define __GameObject__H__

#include "Globals.h"
#include <vector>
#include "MathGeoLib/include/Geometry/AABB.h"
#include "MathGeoLib/include/Geometry/OBB.h"
#include "Component.h"

class Component;
class ComponentTransform;
class ComponentMesh;
class ComponentMaterial;
class ComponentCamera;
class ConfigEditor;
enum ComponentType;


struct json_object_t;
typedef struct json_object_t JSON_Object;

struct json_value_t;
typedef struct json_value_t  JSON_Value;

struct json_array_t;
typedef struct json_array_t  JSON_Array;

class GameObject
{
public:
	GameObject(GameObject* parent);
	virtual ~GameObject();

	void Update(); //TODO
	Component* CreateComponent(ComponentType type);
	Component* GetComponent(ComponentType type) const;
	void DeleteFromComponentList(Component* comp);

	bool IsActive() const;
	void SetActive(bool active);
	bool IsStatic() const;
	void SetStatic(bool staticGO);

	uint GetNumChilds() const;
	GameObject* GetChild(uint i) const;
	GameObject* GetParent() const;
	void SetParent(GameObject* parent);
	void DeleteFromParentList();

	void SetName(const char* new_name);
	const char* GetName()const;

	void GUIHierarchyPrint(int& i, bool& clicked);//Send false on clicked bool
	void DrawComponentsInspector();

	bool IsObjectInHierarchy(GameObject* target, GameObject* curr_node);

	void CenterCameraOnGO() const;

	void UpdateBoundingBox();
	void DrawBoundingBox();

	void Load(JSON_Object* object);
	void Save(JSON_Array* go_array) const;


private:
	bool active = true;
	bool static_go = false;
	bool charge_static = false;
	char name[100];
	GameObject* parent = nullptr;
	
	std::vector<Component*> components;

public:
	//There can be only one component of each type
	ComponentTransform* transform = nullptr;
	//ComponentMesh* mesh = nullptr;
	ComponentMaterial* material = nullptr;
	ComponentCamera* camera = nullptr;
	math::AABB bounding_box;
	math::OBB obb;
	std::vector<GameObject*> childs;
	bool focused = false;
	uint uuid = 0;
};

#endif