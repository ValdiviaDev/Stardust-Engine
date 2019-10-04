#ifndef __PanelConfig_H__
#define __PanelConfig_H__

#include "Panel.h"
#include "globals.h"
#include <vector>
#include "imgui/imgui.h"

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
	void RendererMenu();

	void InputConsole();
	void CheckForCaps();

	void FillFPSVector();
	void FillMSVector();

private:
	vector<float> fps_log;
	vector<float> ms_log;
	const uint perfor_vec_size = 100;
	
	ImGuiTextBuffer imput_log_buff;
	bool imput_log_scroll = false;

};

#endif