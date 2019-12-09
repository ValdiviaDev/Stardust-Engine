#include "NodeKeyInput.h"



NodeKeyInput::NodeKeyInput(int id, const ImVec2& pos) : Node(id, "Event: Input", pos, 0, 1, Node_Type_Event, Node_KeyInput)
{
	for (int i = 0; i < 3; ++i)
		direction[i] = 0.0f;

}


NodeKeyInput::~NodeKeyInput()
{
}

void NodeKeyInput::Update(float dt)
{
}

void NodeKeyInput::Draw()
{
	ImGui::InputFloat3("Direction", direction, 1);
}
