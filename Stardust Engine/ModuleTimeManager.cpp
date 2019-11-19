#include "Application.h"
#include "Globals.h"
#include "ModuleTimeManager.h"



ModuleTimeManager::ModuleTimeManager(Application* app, bool start_enabled) : Module(app, "Time", start_enabled)
{
}


ModuleTimeManager::~ModuleTimeManager()
{
}

bool ModuleTimeManager::CleanUp()
{
	return true;
}
