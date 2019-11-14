#ifndef PAR_SHAPES_IMPLEMENTATION
#define PAR_SHAPES_IMPLEMENTATION
#endif

#include "Par/par_shapes.h"

#include <time.h>

#include "Quadtree.h"
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
#include "ComponentCamera.h"
#include "MaterialImporter.h"
#include "SceneSerialization.h"
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

	//Initialize root GameObject
	CreateRootObject();

	//Baker house create
	scene_gameobject = CreateGameObject(root_object);
	scene_gameobject->SetName("GameObject");
	//scene_gameobject->CreateComponent(Comp_Mesh, "Assets/Meshes/BakerHouse.fbx");
	  

	GameObject* camera = new GameObject(root_object);
	camera->CreateComponent(Comp_Camera);
	camera->SetName("cameraobject");
	
	//Quadtree init TODO test
	BuildQuadtree();


	return true;
}

void ModuleScene::CreateRootObject() {
	root_object = CreateGameObject(nullptr);
	root_object->SetName("root");

}

// Update
update_status ModuleScene::Update(float dt)
{

	root_object->Update();

	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate(float dt) {

	if (want_to_save) {
		want_to_save = false;
		SceneSerialization s;
		s.SaveScene("Assets/Scenes/scene1.json"); 
	}

	if (want_to_load) {
		want_to_load = false;
		SceneSerialization s;
		s.LoadScene("Assets/Scenes/scene1.json");
	}

	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	//Delete GameObjects
	//Releasing the root_object releases all GameObjects, as they are organized as a tree
	RELEASE(root_object);
	RELEASE(quadtree);

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

GameObject* ModuleScene::CreateCubePrimitive()
{
	par_shapes_mesh* cube = par_shapes_create_cube();

	GameObject* cubeGO = CreateGameObject(root_object);
	cubeGO->SetName("Cube");
	cubeGO->CreateComponent(Comp_Mesh, nullptr, PRIMITIVE_CUBE);
	cubeGO->mesh->FillPrimitiveDrawInfo(cube);

	par_shapes_free_mesh(cube);

	return cubeGO;
}

GameObject* ModuleScene::CreateSpherePrimitive(int subdivisions)
{
	par_shapes_mesh* sphere = par_shapes_create_subdivided_sphere(subdivisions);

	GameObject* sphereGO = CreateGameObject(root_object);
	sphereGO->SetName("Sphere");
	sphereGO->CreateComponent(Comp_Mesh, nullptr, PRIMITIVE_SPHERE);
	sphereGO->mesh->FillPrimitiveDrawInfo(sphere);

	par_shapes_free_mesh(sphere);

	return sphereGO;
}

GameObject* ModuleScene::CreatePlanePrimitive(int slices, int stacks)
{
	par_shapes_mesh* plane = par_shapes_create_plane(slices, stacks);

	GameObject* planeGO = CreateGameObject(root_object);
	planeGO->SetName("Plane");
	planeGO->CreateComponent(Comp_Mesh, nullptr, PRIMITIVE_PLANE);
	planeGO->mesh->FillPrimitiveDrawInfo(plane);

	par_shapes_free_mesh(plane);

	return planeGO;
}


void ModuleScene::Draw() {
	DrawGrid();
	DrawGameObjects(root_object);


	//Draw bounding boxes and Quadtree
	glDisable(GL_LIGHTING);
	for (uint i = 0; i < root_object->GetNumChilds(); i++) {
		root_object->GetChild(i)->DrawBoundingBox();
	}

	quadtree->DebugDraw();

	glEnable(GL_LIGHTING);
}

void ModuleScene::DrawGameObjects(GameObject* go)
{
	if (go && go->mesh && go->mesh->IsActive()) { 
		//Matrix
		glPushMatrix();
		float4x4 matrix = go->transform->GetGlobalMatrix();
		glMultMatrixf((GLfloat*)matrix.Transposed().ptr());
		
		//Texture
		if (go->material && go->material->debug_tex_draw) { //Draw texture
			if (go->material->debug_checkers) //Checkers
				glBindTexture(GL_TEXTURE_2D, App->renderer3D->checkersImgId);

			else if(go->material && go->material->GetIfTex()) //Charged texture
				glBindTexture(GL_TEXTURE_2D, go->material->GetTexId());

		}

		//Model
		ComponentMesh* c_mesh = go->mesh;
		geo_info mesh = go->mesh->GetInfo();

		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.id_vertex);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		
		if (!go->mesh->IsPrimitive()) {
			glEnableClientState(GL_NORMAL_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, mesh.id_normal);
			glNormalPointer(GL_FLOAT, 0, NULL);

			if (mesh.uv != nullptr) {
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glBindBuffer(GL_ARRAY_BUFFER, mesh.id_uv);
				glTexCoordPointer(2, GL_FLOAT, 0, NULL);
			}

		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.id_index);

		if(!go->mesh->IsPrimitive())
			glDrawElements(GL_TRIANGLES, mesh.num_index * 3, GL_UNSIGNED_INT, NULL);
		else
			glDrawElements(GL_TRIANGLES, mesh.num_index * 3, GL_UNSIGNED_SHORT, NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		glBindTexture(GL_TEXTURE_2D, 0);

		if(c_mesh->debug_v_norm || c_mesh->debug_f_norm)
			DrawGameObjectsDebug(go);

		glPopMatrix();

	}

	//Look for every other GameObject to draw
	if (go) {
		for (uint i = 0; i < go->GetNumChilds(); i++)
			if (go->GetChild(i) && go->GetChild(i)->IsActive())
				DrawGameObjects(go->GetChild(i));
	}

}

void ModuleScene::DrawGameObjectsDebug(GameObject* go)
{
	//Vertex normals
	geo_info m = go->mesh->GetInfo();
	if (go->mesh->debug_v_norm) {
		glBegin(GL_LINES);
		glColor3f(255.0f, 255.0f, 0.0f); //Yellow

		for (int i = 0; i < m.num_normal * 3; i += 3) {

			//Normalize the vertex normals
			float3 norm = { m.normal[i], m.normal[i + 1], m.normal[i + 2] };
			float  mod = sqrt(norm.x * norm.x + norm.y * norm.y + norm.z * norm.z);
			norm = (norm / mod) * 0.5;

			glVertex3f(m.vertex[i], m.vertex[i + 1], m.vertex[i + 2]);
			glVertex3f(m.vertex[i] + norm.x, m.vertex[i + 1] + norm.y, m.vertex[i + 2] + norm.z);

		}
		glColor3f(255.0f, 255.0f, 255.0f); //White
		glEnd();
	}
	if (go->mesh->debug_f_norm) {


		for (int i = 0; i < m.num_index; i += 3) {
			//Triangle points
			uint index_01 = m.index[i] * 3;
			uint index_02 = m.index[i + 1] * 3;
			uint index_03 = m.index[i + 2] * 3;

			float3 p1 = { m.vertex[index_01], m.vertex[index_01 + 1], m.vertex[index_01 + 2] };
			float3 p2 = { m.vertex[index_02], m.vertex[index_02 + 1], m.vertex[index_02 + 2] };
			float3 p3 = { m.vertex[index_03], m.vertex[index_03 + 1], m.vertex[index_03 + 2] };

			//Calculate face center
			float C1 = (p1.x + p2.x + p3.x) / 3;
			float C2 = (p1.y + p2.y + p3.y) / 3;
			float C3 = (p1.z + p2.z + p3.z) / 3;

			//Calculate Face Normal
			float3 U = { p2 - p1 };
			float3 V = { p3 - p1 };

			float Nx = U.y*V.z - U.z*V.y;
			float Ny = U.z*V.x - U.x*V.z;
			float Nz = U.x*V.y - U.y*V.x;

			//Normalize the face normals
			float  mod = sqrt(Nx * Nx + Ny * Ny + Nz * Nz);
			Nx = (Nx / mod) * 0.5;
			Ny = (Ny / mod) * 0.5;
			Nz = (Nz / mod) * 0.5;

			glBegin(GL_LINES);
			glColor3f(255.0f, 0.0f, 0.0f); //Red

			glVertex3f(C1, C2, C3);
			glVertex3f(C1 + Nx, C2 + Ny, C3 + Nz);

			glColor3f(255.0f, 255.0f, 255.0f); //White
			glEnd();
		}

	}
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
		App->importer->m_debug.clear(); //DEBUG

		UnfocusGameObjects();
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

		//for (int i = 0; i < go->GetNumChilds(); ++i)
			//ChangeGameObjectTexture(tex_path, go->GetChild(i));

	}
}

void ModuleScene::BuildQuadtree()
{
	//If there's quadtree, delete it
	if (quadtree)
		RELEASE(quadtree);
	
	//Create quadtree TODO
	quadtree = new Quadtree();
	AABB root_quad_node = AABB({ -200,-100,-200 }, { 200,100,200 });
	quadtree->Create(root_quad_node);

	//Get a vector of static GameObjects
	vector<GameObject*> static_objects;
	for(int i = 0; i < root_object->GetNumChilds(); ++i)
		GetStaticObjects(static_objects, root_object->childs[i]);

	//Insert all the static GameObjects to the quadtree
	for (int i = 0; i < static_objects.size(); ++i)
		quadtree->Insert(static_objects[i]);

}

void ModuleScene::GetStaticObjects(vector<GameObject*>& static_GOs, GameObject* static_candidate)
{
	if (static_candidate->IsStatic())
		static_GOs.push_back(static_candidate);

	for (int i = 0; i < static_candidate->GetNumChilds(); ++i)
		GetStaticObjects(static_GOs, static_candidate->childs[i]);
}


void ModuleScene::FocusGameObject(GameObject* focused, GameObject* root) {

	if (root->GetNumChilds() > 0) {

		for (std::vector<GameObject*>::const_iterator it = root->childs.begin(); it < root->childs.end(); it++) {

			if ((*it) == focused) {
				(*it)->focused = true;
				focused_object = (*it);
				LOG("%s focused", (*it)->GetName());
			}
			else {
				(*it)->focused = false;
				LOG("%s NOT focused", (*it)->GetName());
			}
			FocusGameObject(focused, (*it));
		}
	}
}

void ModuleScene::UnfocusGameObjects()
{
	if(focused_object)
		focused_object->focused = false;
	focused_object = nullptr;
}



GameObject* ModuleScene::GetFocusedGameObject(GameObject* root) const {

	GameObject* ret = nullptr;

	if (root && root->GetNumChilds() > 0) {

		for (std::vector<GameObject*>::const_iterator it = root->childs.begin(); it < root->childs.end(); it++) {

			if (ret != nullptr)
				return ret;
			

			if ((*it)->focused == true) {
				LOG("%s is the focused GO", (*it)->GetName());
				return *it;

			}
			else {
				LOG("%s isn't the focused GO", (*it)->GetName());
				ret = GetFocusedGameObject(*it);
			}

		}


	}

	return ret;
}

GameObject * ModuleScene::GetGameObjectFromUUID(uint UUID, GameObject* root) const
{

	GameObject* ret = nullptr;


	if (root) {

		if (root->uuid == UUID)
			return root;

		else
		{
			if (root->GetNumChilds() > 0) {

				for (std::vector<GameObject*>::const_iterator it = root->childs.begin(); it < root->childs.end(); it++) {

					if (ret != nullptr)
						return ret;


					if ((*it)->uuid == UUID) {

						return *it;

					}
					else {

						ret = GetGameObjectFromUUID(UUID, *it);
					}

				}


			}
		}
	}
	return ret;
}

void ModuleScene::DestroyGOs() {
	RELEASE(root_object);
}