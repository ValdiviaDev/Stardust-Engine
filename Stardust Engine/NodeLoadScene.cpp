#include "Application.h"
#include "NodeLoadScene.h"
#include "ModuleGui.h"
#include "ModuleScene.h"



NodeLoadScene::NodeLoadScene(int id, const ImVec2& pos) : Node(id, "Action: LoadScene", pos, 1, 1, Node_Type_Action, Func_LoadScene)
{
}


NodeLoadScene::~NodeLoadScene()
{
}

bool NodeLoadScene::Update(float dt, std::vector<GameObject*> BB_objects)
{
	node_state = Node_State_Updating;

	strcpy(App->gui->scene_name, scene_to_load);
	App->scene->want_to_load = true;

	return true;
}

void NodeLoadScene::Draw(std::vector<GameObject*> BB_objects)
{
	ImGui::Text("Scene to load");
	ImGui::InputText("", scene_to_load, IM_ARRAYSIZE(scene_to_load));
}

void NodeLoadScene::Save(JSON_Object* obj) const {

	json_object_set_string(obj, "scene to load", scene_to_load);
}

void NodeLoadScene::Load(JSON_Object* obj) {

	std::strcpy(scene_to_load, json_object_get_string(obj, "scene to load"));
}