#include "Application.h"
#include "ModuleInput.h"
#include "ModuleGui.h"
#include "NodeKeyInput.h"



NodeKeyInput::NodeKeyInput(int id, const ImVec2& pos) : Node(id, "Event: Input", pos, 0, 1, Node_Type_Event, Func_KeyInput)
{
	key_id = SDL_GetScancodeFromName(&key); //TODO: Bug here
}


NodeKeyInput::~NodeKeyInput()
{
}

bool NodeKeyInput::Update(float dt, GameObject* object)
{
	updating = false;

	if (App->input->GetKey(key_id) == KEY_REPEAT)
		updating = true;

	return updating;
}

void NodeKeyInput::Draw()
{
	if (ImGui::InputText("Key", &key, 2)) {
		key_id = SDL_GetScancodeFromName(&key);
	}
}
