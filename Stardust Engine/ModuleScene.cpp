#include <time.h>

#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "Parson/parson.h"

#include "MathGeoLib/include/MathGeoLib.h"
#include <gl/GL.h>

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, "Scene", start_enabled)
{
}

ModuleScene::~ModuleScene()
{}

// Load assets
bool ModuleScene::Start()
{
	LOG("Loading Intro assets");

	//Camera position
	App->camera->Move(vec3(3.0f, 3.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));


	LCG::LCG();

	//Spheres positions and radiuses
	sp1.pos = { 0.0, 0.0, 0.0 };
	sp1.r = 7.0;
	sp2.pos = { 14.1, 0.0, 0.0 };
	sp2.r = 7.0;

	return true;
}

// Update
update_status ModuleScene::Update(float dt)
{

	//Trying out MatGeoLib intersections
	//TryMathGeoLibInters();
	
	//Example calling random float between 0-1 and int between 2 numbers
	//LOG("%f", GetRandomFloat());

	//LOG("%i", GetRandomInt(0, 10));
	

	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{

	return true;
}


void ModuleScene::Draw() {
	glBegin(GL_LINES);
	for (int i = -25; i <= 25; i++) {

		glVertex3f(-25, 0, i);
		glVertex3f(25, 0, i);

		glVertex3f(i, 0, -25);
		glVertex3f(i, 0, 25);

		
		glColor3f(255, 255, 255);
		
	}
	glEnd();
}

float ModuleScene::GetRandomFloat() {

	return lcg.Float(0, 1);;
}

int ModuleScene::GetRandomInt(int a, int b) {

	return lcg.Int(a, b);
}

void ModuleScene::TryMathGeoLibInters()
{
	//Testing sphere intersection
	if (sp1.Intersects(sp2)) {
		LOG("true");
	}
	else {
		LOG("false");
	}
}	



