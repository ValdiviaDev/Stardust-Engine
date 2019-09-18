#include <time.h>

#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(3.0f, 3.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	//Render grid
	Plane plane(0, 1, 0, 0);
	plane.axis = true;
	plane.Render();

	//Render everything on scene
	RenderPrimitives();

	//Title
	char title[80];
	sprintf_s(title, "Stardust Engine %.1f fps", 1/dt);
	App->window->SetTitle(title);


	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::RenderPrimitives()
{
	//Render cubes
	for (p2List_item<Cube>* item = cubesToRender.getFirst(); item; item = item->next)
		item->data.Render();

	//Render cylinders
	for (p2List_item<Cylinder>* item = cylindersToRender.getFirst(); item; item = item->next)
		item->data.Render();

	//Render spheres
	for (p2List_item<Sphere>* item = spheresToRender.getFirst(); item; item = item->next)
		item->data.Render();

}


Cube ModuleSceneIntro::CreateCube(vec3 size, vec3 pos, Color color, float mass, bool draw, bool isPhysicsObject, bool rotate, float angleToRot, vec3  axisToRot)
{
	Cube cube(size.x, size.y, size.z);
	cube.SetPos(pos.x, pos.y, pos.z);
	cube.color = color;

	if (rotate)
		cube.SetRotation(angleToRot, vec3(axisToRot.x, axisToRot.y, axisToRot.z));

	if (draw)
		cubesToRender.add(cube);

	if (isPhysicsObject) 
		App->physics->AddBody(cube, mass);

	return cube;
}

Cylinder ModuleSceneIntro::CreateCylinder(float radius, float height, vec3 pos, Color color, float mass, bool draw, bool isPhysicsObject, bool rotate, float angleToRot, vec3 axisToRot)
{
	Cylinder cylinder(radius, height);
	cylinder.SetPos(pos.x, pos.y, pos.z);
	cylinder.color = color;

	if(rotate)
		cylinder.SetRotation(angleToRot, vec3(axisToRot.x, axisToRot.y, axisToRot.z));

	if (draw)
		cylindersToRender.add(cylinder);

	if (isPhysicsObject)
		App->physics->AddBody(cylinder, mass);

	return cylinder;
}

Sphere ModuleSceneIntro::CreateSphere(float radius, vec3 pos, Color color, float mass, bool draw, bool isPhysicsObject, bool rotate, float angleToRot, vec3 axisToRot)
{
	Sphere sphere(radius);
	sphere.SetPos(pos.x, pos.y, pos.z);
	sphere.color = color;

	if (rotate)
		sphere.SetRotation(angleToRot, vec3(axisToRot.x, axisToRot.y, axisToRot.z));

	if (draw)
		spheresToRender.add(sphere);

	if(isPhysicsObject)
		App->physics->AddBody(sphere, mass);

	return sphere;
}