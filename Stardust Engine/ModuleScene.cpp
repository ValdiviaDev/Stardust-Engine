#ifndef PAR_SHAPES_IMPLEMENTATION
#define PAR_SHAPES_IMPLEMENTATION
#endif

#include "Par/par_shapes.h"

#include <time.h>

#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "Parson/parson.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "GameObject.h"
#include "Component.h"

#include "Glew/include/glew.h"
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
	

	//TESTING, DELETE LATER
	root_object = CreateGameObject(nullptr);
	test = CreateGameObject(root_object);

	test->CreateComponent(Comp_Transform);
	root_object->CreateComponent(Comp_Transform);

	test->transform->SetPosition(float3(2, 0, 0));
	root_object->transform->SetPosition(float3(1, 0, 0));
	
	test->transform->SetRotation(float3(45,0,0));
	root_object->transform->SetRotation(float3(45, 0, 0));


	return true;
}

// Update
update_status ModuleScene::Update(float dt)
{
	
	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	//Delete GameObjects
	for (int i = 0; i < game_objects.size(); ++i)
		RELEASE(game_objects[i]);

	return true;
}

GameObject* ModuleScene::CreateGameObject(GameObject* parent)
{
	GameObject* game_object = new GameObject(parent);
	game_objects.push_back(game_object);

	return game_object;
}


void ModuleScene::Draw() {
	DrawGrid();
}

void ModuleScene::DrawGrid()
{
	glBegin(GL_LINES);

	glColor3f(255.0f, 255.0f, 255.0f);

	for (int i = -25; i <= 25; i++) {

		glVertex3f(-25.0f, 0.0f, i);
		glVertex3f(25.0f, 0.0f, i);

		glVertex3f(i, 0.0f, -25.0f);
		glVertex3f(i, 0.0f, 25.0f);

	}
	glEnd();

}