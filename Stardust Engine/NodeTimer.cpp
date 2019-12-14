#include "NodeTimer.h"



NodeTimer::NodeTimer(int id, const ImVec2& pos) : Node(id, "Event: Timer", pos, 0, 1, Node_Type_Event, Func_Timer)
{
}


NodeTimer::~NodeTimer()
{
}

bool NodeTimer::Update(float dt, std::vector<GameObject*> BB_objects)
{
	updating = false;

	return updating;
}

void NodeTimer::Draw(std::vector<GameObject*> BB_objects)
{
}
