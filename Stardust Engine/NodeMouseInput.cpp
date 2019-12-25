#include "Application.h"
#include "NodeMouseInput.h"
#include "ModuleInput.h"


NodeMouseInput::NodeMouseInput(int id, const ImVec2& pos) : Node(id, "Event: MouseInput", pos, 0, 1, Node_Type_Event, Func_MouseInput)
{
	key_state = KEY_REPEAT;
}


NodeMouseInput::~NodeMouseInput()
{
}

bool NodeMouseInput::Update(float dt, std::vector<GameObject*> BB_objects)
{
	node_state = Node_State_Idle;
	
	if (App->input->GetMouseButton(mouse_butt_id) == key_state)
		node_state = Node_State_Updating;

	return true;
}

void NodeMouseInput::Draw(std::vector<GameObject*> BB_objects)
{
	//Mouse Button
	static const char* mouse_butt[] = { "Left", "Right", "Wheel" };

	if (ImGui::BeginCombo("Mouse button", curr_butt)) {

		if (ImGui::Selectable("Left")) {
			curr_butt = mouse_butt[0];
			mouse_butt_id = 1;
		}
		if (ImGui::Selectable("Right")) {
			curr_butt = mouse_butt[1];
			mouse_butt_id = 3;
		}
		if (ImGui::Selectable("Wheel")) {
			curr_butt = mouse_butt[2];
			mouse_butt_id = 2;
		}

		ImGui::EndCombo();
	}
	
	//Mouse state
	static const char* key_state_str[] = { "Key Down", "Key Repeat", "Key Up" };

	if (ImGui::BeginCombo("Mouse state", curr_state_str)) {

		if (ImGui::Selectable("Key Down")) {
			curr_state_str = key_state_str[0];
			key_state = KEY_DOWN;
		}
		if (ImGui::Selectable("Key Repeat")) {
			curr_state_str = key_state_str[1];
			key_state = KEY_REPEAT;
		}

		if (ImGui::Selectable("Key Up")) {
			curr_state_str = key_state_str[2];
			key_state = KEY_UP;
		}

		ImGui::EndCombo();
	}

}

void NodeMouseInput::Save(JSON_Object* obj) const {
	
	json_object_set_number(obj, "mouse butt id", mouse_butt_id);
	json_object_set_number(obj, "key state", key_state);
	json_object_set_string(obj, "curr butt", curr_butt);
	json_object_set_string(obj, "curr state str", curr_state_str);
}

void NodeMouseInput::Load(JSON_Object* obj) {
	mouse_butt_id = json_object_get_number(obj, "mouse butt id");
	key_state = (KEY_STATE)((int)json_object_get_number(obj, "key state"));

	curr_butt = json_object_get_string(obj, "curr butt");
	curr_state_str = json_object_get_string(obj, "curr state str");
}

