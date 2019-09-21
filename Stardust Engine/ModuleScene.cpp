#include <time.h>

#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"

#include "MathGeoLib/include/MathGeoLib.h"
#include "Glew/include/glew.h"
#include <gl/GL.h>

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
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
	//Grid
	glBegin(GL_LINES); 
		for (int i = -50; i < 50; i++) {

			glVertex3f(-50, 0, i);
			glVertex3f(50, 0, i);

			glVertex3f(i, 0, -50);
			glVertex3f(i, 0, 50);
			
			if (i % 10 == 0) {
				glColor3f(255, 255, 0);
			}
			else {
				glColor3f(255, 0, 255);
			}

		}
		
	glEnd();

	//Trying out MatGeoLib intersections
	//TryMathGeoLibInters();
	
	//Example calling random float between 0-1 and int between 2 numbers
	LOG("%f", GetRandomFloat());

	LOG("%i", GetRandomInt(0, 10));
	

	//Title
	char title[80];
	sprintf_s(title, "Stardust Engine %.1f fps", 1/dt);
	App->window->SetTitle(title);


	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{

	return true;
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
