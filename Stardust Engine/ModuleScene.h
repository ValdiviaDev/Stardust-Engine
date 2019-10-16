#ifndef __ModuleScene_H__
#define __ModuleScene_H__

#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include <vector>

#include "Par/par_shapes.h"


class GameObject;

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	GameObject* CreateGameObject(GameObject* parent);
	GameObject* GetRootGameObject() const;

	GameObject* CreateCubePrimitive();
	GameObject* CreateSpherePrimitive(int subdivisions);
	GameObject* CreatePlanePrimitive(int slices, int stacks);

	void Draw();
	void DrawGameObjects(GameObject* go);
	void DrawGrid();

	void ChangeGameObjectMesh(char* mesh_path);
	void ChangeGameObjectTexture(char* tex_path, GameObject* go);

	void FocusGameObject(GameObject* focused, GameObject* root);


public:
	GameObject* scene_gameobject = nullptr; //Change after first assignment

private:
	GameObject* root_object = nullptr;

};

#endif