#ifndef __PanelInspector_H__
#define __PanelInspector_H__

#include "Panel.h"

class GameObject;

class PanelInspector : public Panel
{
public:
	PanelInspector();
	virtual ~PanelInspector();

	void Draw();

private:

	GameObject* GetFocusedGameObject(GameObject* root);

};

#endif