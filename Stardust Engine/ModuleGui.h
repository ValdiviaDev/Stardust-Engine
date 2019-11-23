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
class PanelEdit;

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

	void SaveSceneMenu();
	void LoadSceneMenu();

	PanelAssets* GetPanelAssets() const;

public:

	vector<UID> loaded_meshes_uuid;
	vector<UID> loaded_textures;
	vector<string> loaded_materials;

	char scene_name[100] = "";

private:
	//Panels
	list<Panel*> panels;
	PanelAbout* p_about = nullptr;
	PanelConsole* p_console = nullptr;
	PanelConfig* p_config = nullptr;
	PanelHierarchy* p_hierarchy = nullptr;
	PanelInspector* p_inspector = nullptr;
	PanelAssets* p_assets = nullptr;
	PanelEdit* p_edit = nullptr;

	update_status decide_if_update = UPDATE_CONTINUE;

	bool save_scene_clicked = false;
	bool load_scene_clicked = false;
	

};

#endif