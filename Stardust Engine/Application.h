#ifndef __Application__H__
#define __Application__H__

#include <list>
#include "Globals.h"
#include "Timer.h"
#include "Module.h"



#include "MathGeoLib/include/Algorithm/Random/LCG.h"

class ModuleWindow;
class ModuleInput;
class ModuleScene;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModuleGui;
class ModuleFileSystem;
class MeshImporter;
class MaterialImporter;
class ModuleTimeManager;
class ModuleResourceManager;
class SceneSerialization;


using namespace std;

class ConfigEditor;

enum EngineState {
	Engine_State_Editor,
	Engine_State_Play,
	Engine_State_Pause,
};


struct Hardware_Info {
	int CPU_core_num = 0;
	int cache_line_size = 0;
	int RAM_mb = 0;

	bool using_AVX = false;
	bool using_AVX2 = false;
	bool using_3DNow = false;
	bool using_AltiVec = false;
	bool using_MMX = false;
	bool using_RDTSC = false;
	bool using_SSE = false;
	bool using_SSE2 = false;
	bool using_SSE3 = false;
	bool using_SSE41 = false;
	bool using_SSE42 = false;
};

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleScene* scene;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleGui* gui;
	ModuleFileSystem* fs;
	ModuleTimeManager* time;
	ModuleResourceManager* resources;
	MaterialImporter* mat_import;
	MeshImporter* mesh_import;
	SceneSerialization* scene_serialization;

private:

	Timer	ms_timer;
	float	dt;
	float curr_dt;
	int		last_frame_ms;
	int		capped_ms;
	int		cap = 60;
	bool	fps_capped = true;

	EngineState engine_state = Engine_State_Editor;
	

	list<Module*> list_modules;

	Hardware_Info h_info;

	LCG lcg;

public:
	string organization_name;
	string app_name;


public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	void RequestBrowser(const char* website) const;
	float GetFPS() const;
	float GetMS() const;
	float Getdt() const;
	int GetFPSCap() const;
	void SetFPSCap(int cap);
	bool GetIfFPSCapping() const;
	void SetIfFPSCapping(bool isCap);

	void SaveHardwareInfo();
	Hardware_Info GetHardwareInfo() const;
	void SetEngineState(EngineState state);
	EngineState GetEngineState() const;

	bool Play();
	void Pause();
	void Stop();

	void SaveConfig() const;
	void LoadConfig();

	void SetAllConfigToDefault();

	u32 GenerateUUID();
	
private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

//Get App from everywhere in the code
extern Application* App;

#endif