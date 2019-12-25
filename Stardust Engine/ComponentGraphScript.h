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
	void LoadBlackBoard();

	std::vector<GameObject*> GetBlackboard()const;
	void AddReferenceToBlackboard(GameObject* ref); //To use when the user adds something via engine
	void ForceAddReferenceToBlackboard(GameObject* ref); //Only to use via code
	bool DeleteGameObjectFromBlackboard(GameObject* to_delete);
	void HideOtherGraphsFromSameObject();

	void NewScriptMenu();
public:
	UID uuid_script = 0;
	char script_name[100] = "Default script";

private:
	bool show_graph = false;
	std::vector<GameObject*> BB_objects;
	std::vector<UID> BB_uids_load;
	bool new_script_clicked = false;
	
};

#endif