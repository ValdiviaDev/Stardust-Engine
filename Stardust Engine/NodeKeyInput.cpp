#include "Application.h"
#include "ModuleInput.h"
#include "ModuleGui.h"
#include "NodeKeyInput.h"



NodeKeyInput::NodeKeyInput(int id, const ImVec2& pos) : Node(id, "Event: Input", pos, 0, 1, Node_Type_Event, Node_KeyInput)
{
	for (int i = 0; i < 3; ++i)
		direction[i] = 0.0f;

	key_id = SDL_GetScancodeFromName(&key); //TODO: Bug here
}


NodeKeyInput::~NodeKeyInput()
{
}

bool NodeKeyInput::Update(float dt)
{
	updating = false;

	if (App->input->GetKey(key_id) == KEY_REPEAT) {
		App->gui->AddLogToConsole("KEY PRESSED IN GRAPH NODE");
		updating = true;

	}

	return updating;
}

void NodeKeyInput::Draw()
{
	if (ImGui::InputText("Key", &key, 2)) {
		key_id = SDL_GetScancodeFromName(&key);
	}

	//ImGui::InputFloat3("Direction", direction, 1);
}
