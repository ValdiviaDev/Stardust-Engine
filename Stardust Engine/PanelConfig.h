#ifndef __PANEL_CONFIG_H__
#define __PANEL_CONFIG_H__

#include "Panel.h"
#include "globals.h"
#include <vector>

using namespace std;

class PanelConfig : public Panel
{
public:
	PanelConfig();
	virtual ~PanelConfig();

	void Draw();

	void ApplicationMenu();
	void WindowMenu();
	void FileSystemMenu();
	void InputMenu();
	void HardwareMenu();

	void FillFPSVector();
	void FillMSVector();

private:

	vector<float> fps_log;
	vector<float> ms_log;
	const uint perfor_vec_size = 70;
};

#endif