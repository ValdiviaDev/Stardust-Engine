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
#include "ComponentGraphScript.h"
#include "MaterialImporter.h"
#include "SceneSerialization.h"
#include "Glew/include/glew.h"
#include <gl/GL.h>
#include "ModuleGui.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"
#include "ModuleResourceManager.h"
#include "ModuleInput.h"
#include "ModuleFileSystem.h"
#include "ModuleCamera3D.h"


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

	//If the main scene exists, charge it
	string main_scene_path = ASSETS_SCENE_FOLDER + (string)"main scene.json";
	if (App->fs->Exists(main_scene_path.c_str())) {
		App->scene_serialization->LoadScene(main_scene_path.c_str());
	}
	else {
		//Charge street envoirment and camera, and save it a s main scene
		App->scene_serialization->LoadSceneFromMesh("Assets/Meshes/Street environment_V01.FBX");
		CreateCamera(true);

		App->scene_serialization->SaveScene(main_scene_path.c_str());
	}
	
	//Quadtree init
	BuildQuadtree();

	//If in GAME_MODE, put the application view in the game camera
#ifdef GAME_MODE
	App->camera->current_cam = GetMainCamera();
#endif

	return true;
}

void ModuleScene::CreateRootObject() {
	root_object = CreateGameObject(nullptr);
	root_object->SetName("root");

}

GameObject* ModuleScene::CreateCamera(bool is_main_camera)
{
	GameObject* camera = new GameObject(root_object);
	ComponentCamera* cam = (ComponentCamera*)camera->CreateComponent(Comp_Camera);
	camera->SetName("camera");

	if(is_main_camera)
		SetMainCamera(cam);

	return camera;
}

ComponentCamera * ModuleScene::GetMainCamera() const
{
	return main_camera;
}

void ModuleScene::SetMainCamera(ComponentCamera* cam)
{
	if (cam == nullptr)
		main_camera = nullptr;
	else {
		if (main_camera)
			main_camera->main_camera = false;

		main_camera = cam;
		main_camera->main_camera = true;
	}
}

// Update
update_status ModuleScene::Update(float dt)
{
	//GameObjexts update
	root_object->Update(dt);

	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
		if (focused_object) {
			GO_to_delete = focused_object;
			want_to_delete_go = true;
		}

	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate(float dt) {

	//Various events to execute via some flags
	if (rebuild_quadtree) {
		rebuild_quadtree = false;

		BuildQuadtree();
	}

#ifndef GAME_MODE
	if (want_to_save) {
		want_to_save = false;

		std::string aux = ASSETS_SCENE_FOLDER;
		aux += App->gui->scene_name;
		aux.append(".json");

		App->scene_serialization->SaveScene(aux.c_str());
	}

	if (want_to_load) {
		want_to_load = false;

		std::string aux = ASSETS_SCENE_FOLDER;
		aux += App->gui->scene_name;
		aux.append(".json");
		
		App->scene_serialization->LoadScene(aux.c_str());
		rebuild_quadtree = true;
	}
#endif

	if (want_to_delete_go) {
		want_to_delete_go = false;

		//Delete
		if (GO_to_delete) {
			bool go_to_del_is_focused = false;
			if (GO_to_delete == focused_object)
				go_to_del_is_focused = true;

			DeleteGameObject(GO_to_delete);
			GO_to_delete = nullptr;

			if (go_to_del_is_focused)
				focused_object = nullptr;
		}
		else
			App->gui->AddLogToConsole("Couldn't delete GameObject, GO_to_delete is NULL");
	}

	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	//Delete GameObjects
	//Releasing the root_object releases all GameObjects, as they are organised as a tree
	DestroyGOs();
	RELEASE(quadtree);

	return true;
}

GameObject* ModuleScene::CreateGameObject(GameObject* parent)
{
	GameObject* game_object = new GameObject(parent);

	return game_object;
}

GameObject* ModuleScene::GetRootGameObject() const
{
	return root_object;
}

GameObject* ModuleScene::CreateGameObjectByMesh(UID mesh_uuid)
{
	//Load resource mesh
	ResourceMesh* res_mesh;
	res_mesh = (ResourceMesh*)App->resources->Get(mesh_uuid);
	if (res_mesh)
		res_mesh->LoadToMemory();

	//Create GameObject
	GameObject* go = App->scene->CreateGameObject(App->scene->GetRootGameObject());
	go->SetName(std::to_string(mesh_uuid).c_str()); //TODO: put name of the Assimp node name
	ComponentMesh* mesh = (ComponentMesh*)go->CreateComponent(Comp_Mesh);
	mesh->uuid_mesh = mesh_uuid;
	mesh->SetPath(res_mesh->GetFile());
	go->UpdateBoundingBox();
	
	return go;
}

void ModuleScene::AssignMeshToGameObject(UID mesh_uuid)
{
	//If there isn't a mesh, create it
	if(focused_object && !focused_object->GetComponent(Comp_Mesh))
		focused_object->CreateComponent(Comp_Mesh);
	
	if (focused_object) {

		ComponentMesh* mesh = (ComponentMesh*)focused_object->GetComponent(Comp_Mesh);

		if (!mesh->IsPrimitive()) {
			//Unload previous mesh resource
			ResourceMesh* res_mesh;
			res_mesh = (ResourceMesh*)App->resources->Get(mesh->uuid_mesh);
			if (res_mesh)
				res_mesh->UnloadToMemory();

			//Load new mesh resource
			res_mesh = (ResourceMesh*)App->resources->Get(mesh_uuid);
			if (res_mesh)
				res_mesh->LoadToMemory();

			mesh->uuid_mesh = mesh_uuid;
			mesh->SetPath(res_mesh->GetFile());
			focused_object->UpdateBoundingBox();
		}
		else
			App->gui->AddLogToConsole("You cannot change the mesh of a primitive.");
	}
}

void ModuleScene::AssignTexToGameObject(UID tex_uuid)
{
	ResourceTexture* res_tex;
	if (focused_object) {
		//Create material if there isn't one
		ComponentMaterial* mat = (ComponentMaterial*)focused_object->GetComponent(Comp_Material);
		if (!mat)
			mat = (ComponentMaterial*)focused_object->CreateComponent(Comp_Material);
		//Unload resource
		else if (mat->HasTex()) {
			res_tex = (ResourceTexture*)App->resources->Get(mat->uuid_mat);
			res_tex->UnloadToMemory();
		}
		//Load new resource
		res_tex = (ResourceTexture*)App->resources->Get(tex_uuid);
		if (res_tex) {
			res_tex->LoadToMemory();
			//Assign the tex to the material
			mat->uuid_mat = res_tex->GetUID();
			mat->SetPath(res_tex->GetFile());
		}
		App->gui->AddLogToConsole("Put a texture from the assets folder");
	}
	else
		App->gui->AddLogToConsole("Select a GameObject to assign a texture material to it.");
}

GameObject* ModuleScene::CreatePrimitiveObject(PrimitiveType type)
{
	//Resource
	ResourceMesh* r_primitive = App->resources->GetPrimitive(type);
	if(r_primitive)
		r_primitive->LoadToMemory();

	//GameObject and Component Mesh
	GameObject* primitiveGO = CreateGameObject(root_object);
	switch (type) {
	case PRIMITIVE_CUBE:
		primitiveGO->SetName("Cube");
		break;
	case PRIMITIVE_SPHERE:
		primitiveGO->SetName("Sphere");
		break;
	case PRIMITIVE_PLANE:
		primitiveGO->SetName("Plane");
		break;
	}
	
	ComponentMesh* mesh = (ComponentMesh*)primitiveGO->CreateComponent(Comp_Mesh);
	mesh->SetPrimitive(type);
	mesh->uuid_mesh = r_primitive->GetUID();
	primitiveGO->UpdateBoundingBox();

	return primitiveGO;
}


void ModuleScene::Draw() {
#ifndef GAME_MODE
	DrawGrid();
#endif

	DrawGameObjects(root_object);

#ifndef GAME_MODE
	//Draw bounding boxes and Quadtree
	if(App->GetEngineState() == Engine_State_Editor)
		DrawSceneDebug();
#endif
}

void ModuleScene::DrawGameObjects(GameObject* go)
{
	if (go && go->IsActive()) {
		ComponentMesh* c_mesh = (ComponentMesh*)go->GetComponent(Comp_Mesh);
		
		if (c_mesh && c_mesh->IsActive() && c_mesh->uuid_mesh != 0) {
			//Matrix
			glPushMatrix();
			float4x4 matrix = go->transform->GetGlobalMatrix();
			glMultMatrixf((GLfloat*)matrix.Transposed().ptr());

			//Texture
			ComponentMaterial* c_mat = (ComponentMaterial*)go->GetComponent(Comp_Material);
			if (c_mat && c_mat->IsActive()) { //Draw texture
				if (c_mat->debug_checkers) //Checkers
					glBindTexture(GL_TEXTURE_2D, App->renderer3D->checkersImgId);

				else if (c_mat && c_mat->uuid_mat != 0) { //Charged texture
					ResourceTexture* tex = (ResourceTexture*)App->resources->Get(c_mat->uuid_mat);
					glBindTexture(GL_TEXTURE_2D, tex->tex_id);
				}
			}

			//Model
			ResourceMesh* mesh = (ResourceMesh*)App->resources->Get(c_mesh->uuid_mesh);

			glEnableClientState(GL_VERTEX_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, mesh->id_vertex);
			glVertexPointer(3, GL_FLOAT, 0, NULL);

			if (!c_mesh->IsPrimitive()) {
				glEnableClientState(GL_NORMAL_ARRAY);
				glBindBuffer(GL_ARRAY_BUFFER, mesh->id_normal);
				glNormalPointer(GL_FLOAT, 0, NULL);

				if (mesh->uv != nullptr) {
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glBindBuffer(GL_ARRAY_BUFFER, mesh->id_uv);
					glTexCoordPointer(2, GL_FLOAT, 0, NULL);
				}

			}

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_index);

			if (!c_mesh->IsPrimitive())
				glDrawElements(GL_TRIANGLES, mesh->num_index * 3, GL_UNSIGNED_INT, NULL);
			else
				glDrawElements(GL_TRIANGLES, mesh->num_index * 3, GL_UNSIGNED_SHORT, NULL);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);

			glBindTexture(GL_TEXTURE_2D, 0);

			if (c_mesh->debug_v_norm || c_mesh->debug_f_norm)
				DrawGameObjectsDebug(c_mesh, mesh);

			glPopMatrix();

		}
	}

	//Look for every other GameObject to draw
	if (go) {
		for (uint i = 0; i < go->GetNumChilds(); i++)
			if (go->GetChild(i))// && go->GetChild(i)->IsActive())
				DrawGameObjects(go->GetChild(i));
	}

}

void ModuleScene::DrawGameObjectsDebug(ComponentMesh* c_mesh, ResourceMesh* r_mesh)
{
	//Vertex normals
	if (c_mesh->debug_v_norm) {
		glBegin(GL_LINES);
		glColor3f(255.0f, 255.0f, 0.0f); //Yellow

		for (int i = 0; i < r_mesh->num_normal * 3; i += 3) {

			//Normalize the vertex normals
			float3 norm = { r_mesh->normal[i], r_mesh->normal[i + 1], r_mesh->normal[i + 2] };
			float  mod = sqrt(norm.x * norm.x + norm.y * norm.y + norm.z * norm.z);
			norm = (norm / mod) * 0.5;

			glVertex3f(r_mesh->vertex[i], r_mesh->vertex[i + 1], r_mesh->vertex[i + 2]);
			glVertex3f(r_mesh->vertex[i] + norm.x, r_mesh->vertex[i + 1] + norm.y, r_mesh->vertex[i + 2] + norm.z);

		}
		glColor3f(255.0f, 255.0f, 255.0f); //White
		glEnd();
	}

	//Face normals
	if (c_mesh->debug_f_norm) {
		for (int i = 0; i < r_mesh->num_index; i += 3) {
			//Triangle points
			uint index_01 = r_mesh->index[i] * 3;
			uint index_02 = r_mesh->index[i + 1] * 3;
			uint index_03 = r_mesh->index[i + 2] * 3;

			float3 p1 = { r_mesh->vertex[index_01], r_mesh->vertex[index_01 + 1], r_mesh->vertex[index_01 + 2] };
			float3 p2 = { r_mesh->vertex[index_02], r_mesh->vertex[index_02 + 1], r_mesh->vertex[index_02 + 2] };
			float3 p3 = { r_mesh->vertex[index_03], r_mesh->vertex[index_03 + 1], r_mesh->vertex[index_03 + 2] };

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

void ModuleScene::DrawSceneDebug()
{
	glDisable(GL_LIGHTING);
	//Draw bounding boxes
	for (uint i = 0; i < root_object->GetNumChilds(); i++) {
		DrawAABBRecursive(root_object->GetChild(i));
	}

	//Draw quadtree
	if (draw_quadtree)
		quadtree->DebugDraw();

	glEnable(GL_LIGHTING);
}

void ModuleScene::DrawAABBRecursive(GameObject * go)
{
	if (go == focused_object || draw_GO_AABBs)
		go->DrawBoundingBox();

	for (uint i = 0; i < go->GetNumChilds(); i++)
		DrawAABBRecursive(go->GetChild(i));
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

//Quadtree functions ----------------------------------------------

void ModuleScene::BuildQuadtree()
{
	//If there's quadtree, delete it
	if (quadtree)
		RELEASE(quadtree);
	
	//Get a vector of static GameObjects
	static_objects.clear();
	for (int i = 0; i < root_object->GetNumChilds(); ++i)
		GetStaticObjects(root_object->GetChild(i));

	//Calculate quadtree size
	float3 min_point(QUADTREE_MIN_SIZE); //Minimum quadtree size
	float3 max_point(QUADTREE_MAX_SIZE); //Maximum quadtree size
	CalculateQuadtreeSize(min_point, max_point);

	//Create quadtree
	quadtree = new Quadtree();
	AABB root_quad_node = AABB(min_point, max_point);
	quadtree->Create(root_quad_node);

	//Insert all the static GameObjects to the quadtree
	for (int i = 0; i < static_objects.size(); ++i)
		quadtree->Insert(static_objects[i]);

}

void ModuleScene::GetStaticObjects(GameObject* static_candidate)
{
	if (static_candidate->IsStatic())
		static_objects.push_back(static_candidate);

	for (int i = 0; i < static_candidate->GetNumChilds(); ++i)
		GetStaticObjects(static_candidate->GetChild(i));
}

void ModuleScene::CalculateQuadtreeSize(float3& min_point, float3& max_point)
{
	for (int i = 0; i < static_objects.size(); ++i) {
		//Min point
		float3 min_p = static_objects[i]->bounding_box.minPoint;
		if (min_p.x < min_point.x)
			min_point.x = min_p.x;
		if (min_p.y < min_point.y)
			min_point.y = min_p.y;
		if (min_p.z < min_point.z)
			min_point.z = min_p.z;

		//Max point
		float3 max_p = static_objects[i]->bounding_box.maxPoint;
		if (max_p.x > max_point.x)
			max_point.x = max_p.x;
		if (max_p.y > max_point.y)
			max_point.y = max_p.y;
		if (max_p.z > max_point.z)
			max_point.z = max_p.z;
	}
}

void ModuleScene::CheckIfRebuildQuadtree(GameObject* go)
{
	float3 min_point = go->bounding_box.minPoint;
	float3 max_point = go->bounding_box.maxPoint;
	bool rebuild = false;
	
	//Min point
	if (min_point.x < quadtree->min_point.x || min_point.x > quadtree->min_point.x && min_point.x < QUADTREE_MIN_SIZE
		|| min_point.y < quadtree->min_point.y || min_point.y > quadtree->min_point.y && min_point.y < QUADTREE_MIN_SIZE
		|| min_point.z < quadtree->min_point.z || min_point.z > quadtree->min_point.z && min_point.y < QUADTREE_MIN_SIZE)
		rebuild = true;
	//Max point
	if (max_point.x > quadtree->max_point.x || max_point.x < quadtree->max_point.x && max_point.x > QUADTREE_MAX_SIZE
		|| max_point.y > quadtree->max_point.y || max_point.y < quadtree->max_point.y && max_point.y > QUADTREE_MAX_SIZE
		|| max_point.z > quadtree->max_point.z || max_point.z < quadtree->max_point.z && max_point.z > QUADTREE_MAX_SIZE)
		rebuild = true;
	
	if (rebuild)
		BuildQuadtree();
}

bool ModuleScene::EraseObjFromStatic(GameObject* go)
{
	for (std::vector<GameObject*>::const_iterator it = static_objects.begin(); it < static_objects.end(); it++)
		if ((*it) == go) {
			static_objects.erase(it);
			return true;
		}
	return false;
}

// --------------------------------------------------------------------------

void ModuleScene::AllObjectsActive(GameObject* go)
{
	go->SetActive(true);

	for (int i = 0; i < go->GetNumChilds(); ++i)
		AllObjectsActive(go->GetChild(i));
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



GameObject* ModuleScene::GetFocusedGameObject() const {

	return focused_object;
}

GameObject* ModuleScene::GetGameObjectFromUUID(uint UUID, GameObject* root) const
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

void ModuleScene::DeleteReferenceFromGraphs(GameObject* root, GameObject* to_delete)
{
	for (int j = 0; j < root->GetNumComp(); ++j) {
		if (root->GetComponentByIndex(j)->GetType() == Comp_Graph_Script) {
			ComponentGraphScript* comp_g = (ComponentGraphScript*)root->GetComponentByIndex(j);
			comp_g->DeleteGameObjectFromBlackboard(to_delete);
		}
	}

	for (int i = 0; i < root->GetNumChilds(); ++i)
		DeleteReferenceFromGraphs(root->GetChild(i), to_delete);
}

void ModuleScene::DeleteGameObject(GameObject* go)
{
	//Delete references from every gameobject
	for (int i = 0; i < root_object->GetNumChilds(); ++i)
		DeleteReferenceFromGraphs(root_object->GetChild(i), go);

	go->DeleteFromParentList();
	RELEASE(go);
}

void ModuleScene::DestroyGOs() {
	RELEASE(root_object);
}