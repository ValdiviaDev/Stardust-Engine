#ifndef __ModuleScene_H__
#define __ModuleScene_H__

#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include <vector>

#include "Par/par_shapes.h"


class GameObject;
class Quadtree;

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	GameObject* CreateGameObject(GameObject* parent);
	GameObject* GetRootGameObject() const;

	GameObject* CreateCubePrimitive();
	GameObject* CreateSpherePrimitive(int subdivisions);
	GameObject* CreatePlanePrimitive(int slices, int stacks);

	void Draw();
	void DrawGameObjects(GameObject* go);
	void DrawGameObjectsDebug(GameObject* go);
	void DrawSceneDebug();
	void DrawAABBRecursive(GameObject* go);
	void DrawGrid();

	void ChangeGameObjectMesh(char* mesh_path);
	void ChangeGameObjectTexture(char* tex_path, GameObject* go);

	void BuildQuadtree();
	void GetStaticObjects(std::vector<GameObject*>& static_GOs, GameObject* static_candidate);

	void FocusGameObject(GameObject* focused, GameObject* root);
	void UnfocusGameObjects();
	GameObject* GetFocusedGameObject(GameObject* root) const;
	GameObject* GetGameObjectFromUUID(uint UUID, GameObject* root) const; //Send root 1st time
	void CreateRootObject();

	void DeleteGameObject(GameObject* go);
	void DestroyGOs();

public:
	GameObject* scene_gameobject = nullptr; //Change after first assignment

	Quadtree* quadtree = nullptr;
	bool want_to_save = false;
	bool want_to_load = false;
	bool rebuild_quadtree = false;

	bool draw_GO_AABBs = false;
	bool draw_quadtree = false;
private:
	
	GameObject* root_object = nullptr;
	GameObject* focused_object = nullptr;
	bool want_to_delete_go = false;

};

#endif