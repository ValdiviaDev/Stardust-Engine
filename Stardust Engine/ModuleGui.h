#pragma once
#include "Module.h"
#include <list>

class Panel;
class PanelAbout;
class PanelConsole;
class PanelConfig;

using namespace std;

class ModuleGui : public Module
{
public:
	ModuleGui(Application* app, bool start_enabled = true);
	~ModuleGui();

	bool Init();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	update_status HandleMainMenuBar();
	void AddLogToConsole(const char* log);

private:

	//Panels
	list<Panel*> panels;
	PanelAbout* about;
	PanelConsole* console;
	PanelConfig* config;
};

