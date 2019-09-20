#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();


	float GetRandomFloat();
	int GetRandomInt(int a, int b);

	void TryMathGeoLibInters();


private:
	//RNG test
	LCG lcg;

	//Intersections test
	math::Sphere sp1;
	math::Sphere sp2;
};
