#include "Application.h"
#include "ModuleInput.h"
#include "ModuleGui.h"
#include "NodeKeyInput.h"



NodeKeyInput::NodeKeyInput(int id, const ImVec2& pos) : Node(id, "Event: Input", pos, 0, 1, Node_Type_Event, Func_KeyInput)
{
	key_id = 4; //TODO: Bug here
	key_state = KEY_REPEAT;
}


NodeKeyInput::~NodeKeyInput()
{
}

bool NodeKeyInput::Update(float dt, std::vector<GameObject*> BB_objects, uint num_comp_graph)
{
	node_state = Node_State_Idle;

	if (App->input->GetKey(key_id) == key_state)
		node_state = Node_State_Updating;

	return true;
}

void NodeKeyInput::Draw(std::vector<GameObject*> BB_objects)
{
	if (ImGui::InputText("Key", &key, 2)) {
		key_id = SDL_GetScancodeFromName(&key);
	}

	//Key state
	static const char* key_state_str[] = { "Key Down", "Key Repeat", "Key Up" };

	if (ImGui::BeginCombo("Key state", curr_state_str)) {

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

void NodeKeyInput::Save(JSON_Object* obj) const {
	json_object_set_number(obj, "key id", key_id);
	json_object_set_number(obj, "key state", key_state);
	char k[2] = { key };
	std::string aux(k);
	json_object_set_string(obj, "key", aux.c_str());
	json_object_set_string(obj, "curr state str", curr_state_str);

}

void NodeKeyInput::Load(JSON_Object* obj) {


	key_id = json_object_get_number(obj, "key id");
	key_state = (KEY_STATE)((int)json_object_get_number(obj, "key state"));
	curr_state_str = json_object_get_string(obj, "curr state str");

	char k[2];
	std::strcpy(k, json_object_get_string(obj, "key"));
	key = k[0];


}