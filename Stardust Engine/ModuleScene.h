#ifndef __ModuleScene_H__
#define __ModuleScene_H__

#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include <vector>

#include "Par/par_shapes.h"

class GameObject;
class Quadtree;
class ComponentCamera;
class ComponentMesh;
class ResourceMesh;
enum PrimitiveType;

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	//GameObject creation and modification
	GameObject* CreateGameObject(GameObject* parent);
	void CreateRootObject();
	GameObject* GetRootGameObject() const;
	GameObject* CreateGameObjectByMesh(UID mesh_uuid);
	void AssignMeshToGameObject(UID mesh_uuid); //On the focused GO
	void AssignTexToGameObject(UID tex_uuid); //On the focused GO

	GameObject* CreatePrimitiveObject(PrimitiveType type);

	//Draw functions
	void Draw();
	void DrawGameObjects(GameObject* go);
	void DrawGameObjectsDebug(ComponentMesh* c_mesh, ResourceMesh* r_mesh);
	void DrawSceneDebug();
	void DrawAABBRecursive(GameObject* go);
	void DrawGrid();

	//Quadtree functions
	void BuildQuadtree();
	void GetStaticObjects(GameObject* static_candidate);
	void CalculateQuadtreeSize(float3& min_point, float3& max_point);
	void CheckIfRebuildQuadtree(GameObject* go);
	bool EraseObjFromStatic(GameObject* go);
	void AllObjectsActive(GameObject* go);

	//Focus gameobject functions
	void FocusGameObject(GameObject* focused, GameObject* root);
	void UnfocusGameObjects();
	GameObject* GetFocusedGameObject() const;
	GameObject* GetGameObjectFromUUID(uint UUID, GameObject* root) const; //Send root 1st time

	GameObject* CreateCamera(bool is_main_camera = false);
	ComponentCamera* GetMainCamera() const;
	void SetMainCamera(ComponentCamera* cam);

	void DeleteGameObject(GameObject* go);
	void DestroyGOs();

public:
	std::vector<GameObject*> static_objects;

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
	ComponentCamera* main_camera = nullptr;
};

#endif