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

bool NodeKeyInput::Update(float dt, std::vector<GameObject*> BB_objects)
{
	updating = false;

	if (App->input->GetKey(key_id) == key_state)
		updating = true;

	return updating;
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
