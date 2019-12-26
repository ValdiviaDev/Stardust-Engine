#include "Application.h"
#include "NodeMouseMove.h"
#include "ModuleInput.h"


NodeMouseMove::NodeMouseMove(int id, const ImVec2& pos) : Node(id, "Event: MouseMove", pos, 0, 1, Node_Type_Event, Func_MouseMove)
{
}


NodeMouseMove::~NodeMouseMove()
{
}

bool NodeMouseMove::Update(float dt, std::vector<GameObject*> BB_objects, uint num_comp_graph)
{
	node_state = Node_State_Idle;

	if (App->input->GetMouseXMotion() != 0 || App->input->GetMouseYMotion() != 0)
		node_state = Node_State_Updating;

	return true;
}

void NodeMouseMove::Draw(std::vector<GameObject*> BB_objects)
{
}

void NodeMouseMove::Save(JSON_Object* obj) const {

}

void NodeMouseMove::Load(JSON_Object* obj) {

}