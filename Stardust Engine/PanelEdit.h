#ifndef __PanelEdit_H__
#define __PanelEdit_H__

#include "Panel.h"

class PanelEdit : public Panel
{
public:
	PanelEdit();
	virtual ~PanelEdit();
	
	void Draw();
	void ChangeCameraView();

private:
	bool playing = false;

};

#endif