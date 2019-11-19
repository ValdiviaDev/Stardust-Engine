#ifndef __ModuleTimeManager_H__
#define __ModuleTimeManager_H__

#include "Module.h"

class ModuleTimeManager : public Module
{
public:
	ModuleTimeManager(Application* app, bool start_enabled = true);
	~ModuleTimeManager();

	bool CleanUp();

private:

	//Real time clock (Engine time)
	int frame_count = 0; //App graphics frames since game start
	float real_time_since_startup = 0.0f; //Seconds since game start
	float real_dt = 0.0f; //Last frame time expressed in seconds

	//Game clock
	float time = 0.0f; //Seconds since game started
	float time_scale = 1.0f; //Scale at which time si passing
	float dt = 0.0f; //Last frame time expressed in seconds

};

#endif