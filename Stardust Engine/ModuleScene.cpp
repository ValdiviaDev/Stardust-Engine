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
	

	//Initialize root GameObject
	root_object = CreateGameObject(nullptr);
	root_object->SetName("root");

	//Baker house test
	scene_gameobject = CreateGameObject(root_object);
	scene_gameobject->SetName("BakerHouse");
	scene_gameobject->CreateComponent(Comp_Mesh, "Assets/Meshes/BakerHouse.fbx");
	if (scene_gameobject->material)
		scene_gameobject->material->AssignTexture("Baker_house.png");
	for (int i = 0; i < scene_gameobject->GetNumChilds(); ++i)
		if (scene_gameobject->GetChild(i)->material)
			scene_gameobject->GetChild(i)->material->AssignTexture("Baker_house.png");
	
	////TEST-------------------------------------------------
	scene_gameobject->transform->SetPosition(float3(0.0f, 5.0f, 0.0f));
	scene_gameobject->transform->SetRotation(float3(90.0f, 0.0f, 0.0f));
	scene_gameobject->transform->SetScale(float3(5.0f, 5.0f, 5.0f));

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
	//Releasing the root_object releases all GameObjects, as they are organized as a tree
	RELEASE(root_object);

	return true;
}

GameObject* ModuleScene::CreateGameObject(GameObject* parent)
{
	GameObject* game_object = new GameObject(parent);

	return game_object;
}

GameObject * ModuleScene::GetRootGameObject() const
{
	return root_object;
}


void ModuleScene::Draw() {
	DrawGrid();
	DrawGameObjects(root_object);


}

void ModuleScene::DrawGameObjects(GameObject* go)
{
	if (go && go->mesh && go->mesh->IsActive()) { 
		//Matrix
		glPushMatrix();
		float4x4 matrix = go->transform->GetGlobalMatrix();
		glMultMatrixf((GLfloat*)matrix.Transposed().ptr());
		
		//Texture
		if (go->material && go->material->GetIfTex())
			glBindTexture(GL_TEXTURE_2D, go->material->GetTexId());

		//Model
		geo_info mesh = go->mesh->GetInfo();

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

	if (go) {
		for (uint i = 0; i < go->GetNumChilds(); i++)
			if (go->GetChild(i) && go->GetChild(i)->IsActive())
				DrawGameObjects(go->GetChild(i));
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

void ModuleScene::ChangeGameObjectMesh(char* mesh_path)
{
	if (scene_gameobject) {
		//Erase the GameObject from the root vector before deleting the GameObject
		scene_gameobject->DeleteFromParentList();
		RELEASE(scene_gameobject);
	}

	scene_gameobject = CreateGameObject(root_object);
	scene_gameobject->CreateComponent(Comp_Mesh, mesh_path);
	scene_gameobject->SetName("SceneObj");

}

void ModuleScene::ChangeGameObjectTexture(char* tex_path, GameObject* go)
{
	if (go && go->mesh) {
		if (!go->material)
			go->CreateComponent(Comp_Material);

		go->material->AssignTexture(tex_path);

		for (int i = 0; i < go->GetNumChilds(); ++i)
			ChangeGameObjectTexture(tex_path, go->GetChild(i));

	}
}
