#include "Application.h"
#include "ConfigEditor.h"


Application::Application()
{
	last_frame_ms = -1;

	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	scene = new ModuleScene(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	gui = new ModuleGui(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	
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



	ConfigEditor config;
	ConfigEditor module_location("App");

	

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
}

// ---------------------------------------------
void Application::FinishUpdate()
{
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
		ret = (*item)->PreUpdate(dt);
		item++;
	}

	item = list_modules.begin();

	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->Update(dt);
		item++;
	}

	item = list_modules.begin();

	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->PostUpdate(dt);
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
	return ret;
}

void Application::SaveConfig() const {

	
	ConfigEditor config;
	ConfigEditor module_location("App");

	for (list<Module*>::const_iterator item = list_modules.begin(); item != list_modules.end(); item++) {
		
		module_location = module_location.GetModuleLocation((*item)->GetName());
		(*item)->Save(&module_location);
	}
}

void Application:: LoadConfig() {

	ConfigEditor config;
	ConfigEditor module_location("App");

	for (list<Module*>::const_iterator item = list_modules.begin(); item != list_modules.end(); item++) {
		
		module_location = module_location.GetModuleLocation((*item)->GetName());
		(*item)->Load(&module_location);
	}
}


void Application::RequestBrowser(const char* url) const
{
	ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}

float Application::GetFPS() const
{
	return (1.0f/dt);
}

float Application::GetMS() 
{
	return ((float)last_frame_ms);
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}