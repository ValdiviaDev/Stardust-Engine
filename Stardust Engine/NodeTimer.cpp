#include "NodeTimer.h"
#include "GameObject.h"
#include "ComponentGraphScript.h"



NodeTimer::NodeTimer(int id, const ImVec2& pos, uint timer_num) : Node(id, "Event: Timer", pos, 0, 1, Node_Type_Event, Func_Timer)
{
	this->timer_num = timer_num;
}


NodeTimer::~NodeTimer()
{
}

bool NodeTimer::Update(float dt, std::vector<GameObject*> BB_objects, uint num_comp_graph)
{
	node_state = Node_State_Idle;
	static bool make_action = true;

	ComponentGraphScript* comp_gs = BB_objects[0]->GetCompScript(num_comp_graph);

	if (comp_gs->GetNumTimers() < timer_num) {
		comp_gs->CreateNewTimer();
	}

	//Time count
	comp_gs->IncrementTimer(timer_num, dt);

	//Timer check
	float timer = comp_gs->GetTimer(timer_num);
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
	ImGui::Text("Graph timer number ");
	ImGui::SameLine();
	ImGui::TextColored({ 255,255,0,255 }, "%i", timer_num);

	ImGui::Checkbox("Reset", &reset);
	ImGui::Checkbox("Repeat action", &repeat_action);

	ImGui::InputFloat("Time to action", &time, 0.0f, 0.0f, 2);
}



void NodeTimer::Save(JSON_Object* obj) const {

	json_object_set_boolean(obj, "repeat action", repeat_action);
	json_object_set_boolean(obj, "reset", reset);
	json_object_set_number(obj, "time", time);
}

void NodeTimer::Load(JSON_Object* obj) {

	repeat_action = json_object_get_boolean(obj, "repeat action");
	reset = json_object_get_boolean(obj, "reset");
	time = json_object_get_number(obj, "time");

}