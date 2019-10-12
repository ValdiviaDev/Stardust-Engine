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
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"

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
	root_object->SetName("root");

	test = CreateGameObject(root_object);
	test->SetName("test");
	test->CreateComponent(Comp_Mesh, "BakerHouse.fbx");
	if (test->material)
		test->material->AssignTexture("Baker_house.png");
	for (int i = 0; i < test->GetNumChilds(); ++i)
		if (test->GetAChild(i)->material)
			test->GetAChild(i)->material->AssignTexture("Baker_house.png");

	//TEST-------------------------------------------------
	test->transform->SetPosition(float3(0.0f, 0.0f, 0.0f));
	test->transform->SetRotation(float3(0.0f, 0.0f, 0.0f));
	test->transform->SetScale(float3(1.0f, 1.0f, 1.0f));

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
	DrawGameObjects();
}

void ModuleScene::DrawGameObjects()
{
	for (int i = 0; i < game_objects.size(); ++i) {
		if (game_objects[i]->mesh) { 
			glPushMatrix();
			float4x4 matrix = game_objects[i]->transform->GetGlobalMatrix();
			glMultMatrixf((GLfloat*)matrix.Transposed().ptr());
			
			//Texture
			if (game_objects[i]->material && game_objects[i]->material->GetIfTex())
				glBindTexture(GL_TEXTURE_2D, game_objects[i]->material->GetTexId());

			geo_info mesh = game_objects[i]->mesh->GetInfo();

			glEnableClientState(GL_VERTEX_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, mesh.id_vertex);
			glVertexPointer(3, GL_FLOAT, 0, NULL);

			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, mesh.id_uv);
			glTexCoordPointer(2, GL_FLOAT, 0, NULL);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.id_index);

			glDrawElements(GL_TRIANGLES, mesh.num_index * 3, GL_UNSIGNED_INT, NULL);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);

			glBindTexture(GL_TEXTURE_2D, 0);

			glPopMatrix();
		}
	}
	//root_object->GUIHierarchyPrint(i);
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