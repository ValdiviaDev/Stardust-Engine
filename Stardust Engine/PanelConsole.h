#ifndef __PANEL_CONSOLE_H__
#define __PANEL_CONSOLE_H__

#include "Panel.h"
#include "Imgui/imgui.h"

class PanelConsole : public Panel
{
public:
	PanelConsole();
	virtual ~PanelConsole();

	void Draw();
	void AddLog(const char* log);
	void ConsoleClear();

private:
	ImGuiTextBuffer buff;
	bool scroll = false;
};

#endif