#ifndef __ComponentGraphScript__H__
#define __ComponentGraphScript__H__

#include "Component.h"

class ComponentGraphScript : public Component
{
public:
	ComponentGraphScript(GameObject* parent);
	~ComponentGraphScript();

	void Update(float dt);
	void DrawInspector();

public:
	UID uuid_script = 0;

private:
	bool has_script = false;
	bool show_graph = false;

};

#endif