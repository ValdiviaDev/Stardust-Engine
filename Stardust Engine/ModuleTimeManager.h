#ifndef __ModuleTimeManager_H__
#define __ModuleTimeManager_H__

#include "Module.h"

class ModuleTimeManager : public Module
{
public:
	ModuleTimeManager(Application* app, bool start_enabled = true);
	~ModuleTimeManager();

	update_status PostUpdate(float dt);
	bool CleanUp();

	void ResetGameTimer();

	uint GetFrameCount() const;
	float GetRealTimeClock() const;
	float GetRealdt() const;
	float GetGameTime() const;
	float GetTimeScale() const;
	float GetGamedt() const;

private:

	//Real time clock (Engine time)
	uint frame_count = 0; //App graphics frames since game start
	float real_time = 0.0f; //Seconds since engine has been opened
	float real_dt = 0.0f; //Last frame time expressed in seconds

	//Game clock
	float time = 0.0f; //Seconds since game started
	float time_scale = 1.0f; //Scale at which time si passing
	float dt = 0.0f; //Last frame time expressed in seconds

};

#endif