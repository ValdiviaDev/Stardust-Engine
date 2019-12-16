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

	void AddReferenceToBlackboard(GameObject* ref);
	bool DeleteGameObjectFromBlackboard(GameObject* to_delete);
	void HideOtherGraphsFromSameObject();

public:
	UID uuid_script = 0;

private:
	bool has_script = false;
	bool show_graph = false;

	std::vector<GameObject*> BB_objects;
};

#endif