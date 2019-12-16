#include "NodeTimer.h"



NodeTimer::NodeTimer(int id, const ImVec2& pos) : Node(id, "Event: Timer", pos, 0, 1, Node_Type_Event, Func_Timer)
{
}


NodeTimer::~NodeTimer()
{
}

bool NodeTimer::Update(float dt, std::vector<GameObject*> BB_objects)
{
	node_state = Node_State_Idle;
	static bool make_action = true;

	//Time count
	timer += dt;

	if (timer >= time) {
		if (make_action)
			node_state = Node_State_Updating;

		if (!repeat_action)
			make_action = false;
		else
			make_action = true;

		if (reset) {
			make_action = true;
			timer = 0.0f;
		}

	}
	else
		node_state = Node_State_ToUpdate;

	return true;
}

void NodeTimer::Draw(std::vector<GameObject*> BB_objects)
{
	ImGui::Checkbox("Reset", &reset);
	ImGui::Checkbox("Repeat action", &repeat_action);

	ImGui::InputFloat("Time", &time, 0.0f, 0.0f, 2);

	ImGui::Text("Time since action:");
	ImGui::TextColored({ 255,255,0,255 }," %0.2f", timer);
}
