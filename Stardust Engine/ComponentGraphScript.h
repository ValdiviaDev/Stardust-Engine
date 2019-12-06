#ifndef __ComponentGraphScript__H__
#define __ComponentGraphScript__H__

#include "Component.h"

class ComponentGraphScript : public Component
{
public:
	ComponentGraphScript(GameObject* parent);
	~ComponentGraphScript();

	void DrawInspector();

private:
	bool has_script = false;

};

#endif