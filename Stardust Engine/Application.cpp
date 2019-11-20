#include "Application.h"
#include "ConfigEditor.h"


Application::Application()
{
	last_frame_ms = -1;
	if(fps_capped)
		capped_ms = 1000 / cap;
	SaveHardwareInfo();

	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	scene = new ModuleScene(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	gui = new ModuleGui(this);
	fs = new ModuleFileSystem(this);
	time = new ModuleTimeManager(this);
	mat_import = new MaterialImporter();
	mesh_import = new MeshImporter();

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(fs);
	AddModule(time);

	// Scene
	AddModule(scene);

	//imGui
	AddModule(gui);

	// Renderer last!
	AddModule(renderer3D);
}

Application::~Application()
{
	list<Module*>::reverse_iterator item = list_modules.rbegin();

	while(item != list_modules.rend())
	{
		delete *item;
		item++;
	}
}

bool Application::Init()
{
	bool ret = true;

	// Call Init() in all modules
	list<Module*>::const_iterator item = list_modules.begin();



	
	ConfigEditor module_location("config.json", "App");

	LCG::LCG();

	for (item; item != list_modules.end() && ret; item++) 
	{
		module_location = module_location.GetModuleLocation((*item)->GetName());
		ret = (*item)->Init(&module_location);
		
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = (*item)->Start();
		item++;
	}
	
	LoadConfig();

	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();

	//Grab dt depending if we're in editor or in game
	switch (engine_state) {
	case Engine_State_Editor:
		curr_dt = dt;
		break;
	case Engine_State_Play:
	case Engine_State_Pause:
		curr_dt = time->GetGamedt();
	}

}

// ---------------------------------------------
void Application::FinishUpdate()
{
	last_frame_ms = ms_timer.Read();

	if (fps_capped) {
		if (capped_ms > 0 && (last_frame_ms < capped_ms))
			SDL_Delay(capped_ms - last_frame_ms);
	}
	last_frame_ms = ms_timer.Read();
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();

	list<Module*>::const_iterator item = list_modules.begin();
	
	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->PreUpdate(curr_dt);
		item++;
	}

	item = list_modules.begin();

	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->Update(curr_dt);
		item++;
	}

	item = list_modules.begin();

	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->PostUpdate(curr_dt);
		item++;
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	list<Module*>::reverse_iterator item = list_modules.rbegin();

	while(item != list_modules.rend() && ret == true)
	{
		ret = (*item)->CleanUp();
		item++;
	}
	RELEASE(mat_import);
	RELEASE(mesh_import);

	return ret;
}

void Application::SaveConfig() const {

	
	
	ConfigEditor module_location("config.json", "App");

	for (list<Module*>::const_iterator item = list_modules.begin(); item != list_modules.end(); item++) {
		
		module_location = module_location.GetModuleLocation((*item)->GetName());
		(*item)->Save(&module_location);
		
	}
	module_location.SaveFile("config.json");
}

void Application:: LoadConfig() {

	
	ConfigEditor module_location("config.json", "App");

	for (list<Module*>::const_iterator item = list_modules.begin(); item != list_modules.end(); item++) {
		
		module_location = module_location.GetModuleLocation((*item)->GetName());
		(*item)->Load(&module_location);
	}
}

void Application::SetAllConfigToDefault() {

	
	ConfigEditor module_location("config.json", "App");

	for (list<Module*>::const_iterator item = list_modules.begin(); item != list_modules.end(); item++) {

		module_location = module_location.GetModuleLocation((*item)->GetName());
		(*item)->SetDefaultConfig();
		(*item)->Save(&module_location);
	}
	module_location.SaveFile("config.json");

	LoadConfig();
}


void Application::RequestBrowser(const char* url) const
{
	ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}

float Application::GetFPS() const
{
	return (1.0f/dt);
}

float Application::GetMS() const 
{
	return ((float)last_frame_ms);
}

float Application::Getdt() const
{
	return dt;
}

int Application::GetFPSCap() const
{
	return cap;
}

void Application::SetFPSCap(int cap)
{
	if(cap != 0)
		capped_ms = 1000 / cap;
	else
		capped_ms = 1000 / this->cap;
}

bool Application::GetIfFPSCapping() const
{
	return fps_capped;
}

void Application::SetIfFPSCapping(bool isCap)
{
	fps_capped = isCap;
}

void Application::SaveHardwareInfo()
{
	h_info.CPU_core_num = SDL_GetCPUCount();
	h_info.cache_line_size = SDL_GetCPUCacheLineSize();
	h_info.RAM_mb = SDL_GetSystemRAM();

	h_info.using_AVX = SDL_HasAVX();
	h_info.using_AVX2 = SDL_HasAVX2();
	h_info.using_3DNow = SDL_Has3DNow();
	h_info.using_AltiVec = SDL_HasAltiVec();
	h_info.using_MMX = SDL_HasMMX();
	h_info.using_RDTSC = SDL_HasRDTSC();
	h_info.using_SSE = SDL_HasSSE();
	h_info.using_SSE2 = SDL_HasSSE2();
	h_info.using_SSE3 = SDL_HasSSE3();
	h_info.using_SSE41 = SDL_HasSSE41();
	h_info.using_SSE42 = SDL_HasSSE42();
}

Hardware_Info Application::GetHardwareInfo() const
{
	return h_info;
}

void Application::SetEngineState(EngineState state)
{
	engine_state = state;
}

EngineState Application::GetEngineState() const
{
	return engine_state;
}

bool Application::Play()
{
	switch(engine_state){
	case Engine_State_Editor:
		if (scene->GetMainCamera() != nullptr) {
			//Change camera view
			camera->current_cam = scene->GetMainCamera();
			renderer3D->RecalculateProjMat();

			//Save scene tmp
			std::string aux = LIBRARY_FOLDER;
			aux.append("tmp_scene.json");
			tmp_scene.SaveScene(aux.c_str());
			scene->rebuild_quadtree = true;

			SetEngineState(Engine_State_Play);

			return true;
		}
		else
			gui->AddLogToConsole("ERROR: You don't have a Main Camera GameObject on the scene!");
		
		break;
	}

	return false;
}

void Application::Pause()
{
	switch (engine_state) {
	case Engine_State_Play:
		SetEngineState(Engine_State_Pause);
		break;
	case Engine_State_Pause:
		SetEngineState(Engine_State_Play);
		break;
	}
}

void Application::Stop()
{
	switch (engine_state) {
	case Engine_State_Play:
	case Engine_State_Pause:
		//Change camera view
		camera->current_cam = App->camera->engine_cam;
		renderer3D->RecalculateProjMat();

		SetEngineState(Engine_State_Editor);

		//Load scene tmp
		std::string aux = LIBRARY_FOLDER;
		aux.append("tmp_scene.json");
		tmp_scene.LoadScene(aux.c_str());
		scene->rebuild_quadtree = true;

		time->ResetGameTimer();
		
		break;
	}
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}


u32 Application::GenerateUUID() {
	
	//random between 0 and max_int
	return lcg.Int();
}