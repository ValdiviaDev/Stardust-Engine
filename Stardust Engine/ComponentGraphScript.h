#ifndef __ComponentGraphScript__H__
#define __ComponentGraphScript__H__

#include "Component.h"
#include <vector>

class ComponentGraphScript : public Component
{
public:
	ComponentGraphScript(GameObject* parent);
	~ComponentGraphScript();

	void Update(float dt);
	void DrawInspector();
	bool HasScript();

	void Save(JSON_Array* comp_array) const;
	void Load(JSON_Object* comp_obj);
	void SaveScriptFile(UID uuid) const;

	std::vector<GameObject*> GetBlackboard()const;
	void AddReferenceToBlackboard(GameObject* ref); //To use when the user adds something via engine
	void ForceAddReferenceToBlackboard(GameObject* ref); //Only to use via code
	bool DeleteGameObjectFromBlackboard(GameObject* to_delete);
	void HideOtherGraphsFromSameObject();

public:
	UID uuid_script = 0;

private:
	bool show_graph = false;
	std::vector<GameObject*> BB_objects;
};

#endif