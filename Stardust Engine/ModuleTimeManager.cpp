#include "Application.h"
#include "Globals.h"
#include "ModuleTimeManager.h"



ModuleTimeManager::ModuleTimeManager(Application* app, bool start_enabled) : Module(app, "Time", start_enabled)
{
}


ModuleTimeManager::~ModuleTimeManager()
{
}

update_status ModuleTimeManager::PostUpdate(float dt)
{
	frame_count++;

	real_dt = dt;
	real_time += real_dt;

	switch (App->GetEngineState()) {
	case Engine_State_Editor:
		break;

	case Engine_State_Play:
		this->dt = real_dt * time_scale;
		time += this->dt;
		break;

	case Engine_State_Pause:
		this->dt = 0.0f;
		break;

	}

	return UPDATE_CONTINUE;
}

bool ModuleTimeManager::CleanUp()
{
	return true;
}

void ModuleTimeManager::ResetGameTimer()
{
	time = 0.0f;
	dt = 0.0f;
}

uint ModuleTimeManager::GetFrameCount() const
{
	return frame_count;
}

float ModuleTimeManager::GetRealTimeClock() const
{
	return real_time;
}

float ModuleTimeManager::GetRealdt() const
{
	return real_dt;
}

float ModuleTimeManager::GetGameTime() const
{
	return time;
}

float ModuleTimeManager::GetTimeScale() const
{
	return time_scale;
}

float ModuleTimeManager::GetGamedt() const
{
	return dt;
}
