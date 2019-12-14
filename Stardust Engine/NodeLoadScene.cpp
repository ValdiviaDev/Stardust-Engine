#include "NodeLoadScene.h"



NodeLoadScene::NodeLoadScene(int id, const ImVec2& pos) : Node(id, "Action: LoadScene", pos, 1, 1, Node_Type_Action, Func_LoadScene)
{
}


NodeLoadScene::~NodeLoadScene()
{
}

bool NodeLoadScene::Update(float dt, std::vector<GameObject*> BB_objects)
{
	updating = false;

	return updating;
}

void NodeLoadScene::Draw(std::vector<GameObject*> BB_objects)
{
}
