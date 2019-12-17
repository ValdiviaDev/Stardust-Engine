#include "NodeInstantiateObject.h"



NodeInstantiateObject::NodeInstantiateObject(int id, const ImVec2& pos) : Node(id, "Event: InstantiateObject", pos, 1, 1, Node_Type_Action, Func_InstObject)
{
}


NodeInstantiateObject::~NodeInstantiateObject()
{
}

bool NodeInstantiateObject::Update(float dt, std::vector<GameObject*> BB_objects)
{
	node_state = Node_State_Idle;


	return true;
}

void NodeInstantiateObject::Draw(std::vector<GameObject*> BB_objects)
{

	ImGui::Checkbox("Get transform from reference", &get_ref_trans);
	ImGui::Checkbox("Get transform fron instance", &get_inst_trans);


	if (get_ref_trans) {
		//GameObject reference
		DrawObjectsInstance(BB_objects);
	}

}
