#include "NodeDeleteObject.h"



NodeDeleteObject::NodeDeleteObject(int id, const ImVec2& pos) : Node(id, "Action: DeleteObject", pos, 1, 1, Node_Type_Action, Func_DelObject)
{
}


NodeDeleteObject::~NodeDeleteObject()
{
}

bool NodeDeleteObject::Update(float dt, std::vector<GameObject*> BB_objects)
{
	return false;
}

void NodeDeleteObject::Draw(std::vector<GameObject*> BB_objects)
{
}
