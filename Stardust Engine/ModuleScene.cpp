#include <time.h>

#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
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

	return true;
}

// Update
update_status ModuleScene::Update(float dt)
{

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