#pragma once
#include "Module.h"
#include <list>

class Panel;
class PanelAbout;
class PanelConsole;
class PanelConfig;
class ModuleGui;

using namespace std;

class ModuleGui : public Module
{
public:
	ModuleGui(Application* app, bool start_enabled = true);
	~ModuleGui();

	bool Init(ConfigEditor* config);
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	update_status HandleMainMenuBar();
	void AddLogToConsole(const char* log);

	/*void Load(ConfigEditor* config);
	void Save(ConfigEditor* config) const;*/

private:

	//Panels
	list<Panel*> panels;
	PanelAbout* p_about;
	PanelConsole* p_console;
	PanelConfig* p_config;
};

