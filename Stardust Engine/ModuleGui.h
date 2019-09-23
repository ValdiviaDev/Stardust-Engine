#pragma once
#include "Module.h"

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
	void HandleAboutWindow();

private:
	bool show_app_console = true;
	bool show_app_config = true;
	bool show_about_window = false;
};

