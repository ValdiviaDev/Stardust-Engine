#ifndef __PANEL_CONFIG_H__
#define __PANEL_CONFIG_H__

#include "Panel.h"
#include <vector>

using namespace std;

class PanelConfig : public Panel
{
public:
	PanelConfig();
	virtual ~PanelConfig();

	void Draw();

	void FillFPSVector();

private:
	vector<float> fps_log;

};

#endif