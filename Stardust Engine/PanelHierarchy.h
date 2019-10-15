#ifndef __PanelHierarchy_H__
#define __PanelHierarchy_H__

#include "Panel.h"

class GameObject;

class PanelHierarchy : public Panel
{
public:
	PanelHierarchy();
	virtual ~PanelHierarchy();

	void Draw();
	void DrawGOHierarchy(GameObject* go, int& node_index);

private:

};

#endif