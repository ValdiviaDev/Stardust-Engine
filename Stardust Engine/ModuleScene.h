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

	void ChangeGameObjectTexture(char* tex_path, GameObject* go);

	void BuildQuadtree();
	void GetStaticObjects(GameObject* static_candidate);
	void CalculateQuadtreeSize(float3& min_point, float3& max_point);
	void CheckIfRebuildQuadtree(GameObject* go);
	bool EraseObjFromStatic(GameObject* go);
	void AllObjectsActive(GameObject* go);

	void FocusGameObject(GameObject* focused, GameObject* root);
	void UnfocusGameObjects();
	GameObject* GetFocusedGameObject() const;
	GameObject* GetGameObjectFromUUID(uint UUID, GameObject* root) const; //Send root 1st time
	void CreateRootObject();

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