#pragma once

#include <list>
#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleGui.h"

using namespace std;

class ConfigEditor;

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

private:

	Timer	ms_timer;
	float	dt;
	int		last_frame_ms;

	list<Module*> list_modules;

	Hardware_Info h_info;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	void RequestBrowser(const char* website) const;
	float GetFPS() const;
	float GetMS();

	void SaveHardwareInfo();
	Hardware_Info GetHardwareInfo();

	void SaveConfig() const;
	void LoadConfig();


private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

//Get App from everywhere in the code
extern Application* App;