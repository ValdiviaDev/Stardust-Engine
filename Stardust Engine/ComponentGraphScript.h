#ifndef __ComponentGraphScript__H__
#define __ComponentGraphScript__H__

#include "Component.h"
#include <vector>

class ComponentGraphScript : public Component
{
public:
	ComponentGraphScript(GameObject* parent, uint script_num);
	~ComponentGraphScript();

	void Update(float dt);
	void DrawInspector();
	bool HasScript();
	uint GetCompSriptNum() const;

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

	uint CreateNewTimer();
	void IncrementTimer(uint idx, float dt);
	uint GetNumTimers() const;
	float GetTimer(uint idx) const;

public:
	UID uuid_script = 0;
	char script_name[100] = "Default script";

private:
	uint script_num;
	bool show_graph = false;
	std::vector<GameObject*> BB_objects;
	std::vector<UID> BB_uids_load;
	bool new_script_clicked = false;
	
	std::vector<float> timers;
};

#endif