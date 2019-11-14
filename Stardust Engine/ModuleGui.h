#ifndef __ModuleGui__H__
#define __ModuleGui__H__

#include "Module.h"
#include <list>

class Panel;
class PanelAbout;
class PanelConsole;
class PanelConfig;
class ModuleGui;
class PanelHierarchy;
class PanelInspector;
class PanelAssets;

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

	void Draw();

	void HandleMainMenuBar();
	void AddLogToConsole(const char* log);
	void ResizePanels();
	bool IsMouseHoveringWindow();

	void HandleGuizmo();

	/*void Load(ConfigEditor* config);
	void Save(ConfigEditor* config) const;*/
	vector<string> loaded_meshes;
	vector<string> loaded_materials;

private:
	//Panels
	list<Panel*> panels;
	PanelAbout* p_about = nullptr;
	PanelConsole* p_console = nullptr;
	PanelConfig* p_config = nullptr;
	PanelHierarchy* p_hierarchy = nullptr;
	PanelInspector* p_inspector = nullptr;
	PanelAssets* p_assets = nullptr;

	update_status decide_if_update = UPDATE_CONTINUE;

};

#endif