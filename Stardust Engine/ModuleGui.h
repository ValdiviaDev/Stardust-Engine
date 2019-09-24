#pragma once
#include "Module.h"
#include <list>
#include "Panel.h"
#include "PanelAbout.h"

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
	void HandleConsoleWindow();
	void HandleConfigWindow();

private:
	bool show_app_console = true;
	bool show_app_config = true;

	//Panels
	list<Panel*> panels;
	PanelAbout* about;
};

