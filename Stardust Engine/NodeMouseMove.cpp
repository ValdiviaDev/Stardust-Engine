#include "Application.h"
#include "NodeMouseMove.h"
#include "ModuleInput.h"


NodeMouseMove::NodeMouseMove(int id, const ImVec2& pos) : Node(id, "Event: MouseMove", pos, 0, 1, Node_Type_Event, Func_MouseMove)
{
}


NodeMouseMove::~NodeMouseMove()
{
}

bool NodeMouseMove::Update(float dt, std::vector<GameObject*> BB_objects)
{
	updating = false;

	if (App->input->GetMouseXMotion() != 0 || App->input->GetMouseYMotion() != 0)
		updating = true;

	return updating;
}

void NodeMouseMove::Draw()
{
}
