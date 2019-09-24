#pragma once
#include "Module.h"
#include <list>
#include "Panel.h"
#include "PanelAbout.h"
#include "PanelConsole.h"
#include "PanelConfig.h"

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

private:

	//Panels
	list<Panel*> panels;
	PanelAbout* about;
	PanelConsole* console;
	PanelConfig* config;
};

