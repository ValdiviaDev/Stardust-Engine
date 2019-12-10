#include "Application.h"
#include "NodeMouseInput.h"
#include "ModuleInput.h"


NodeMouseInput::NodeMouseInput(int id, const ImVec2& pos) : Node(id, "Event: MouseInput", pos, 0, 1, Node_Type_Event, Func_MouseInput)
{
	
}


NodeMouseInput::~NodeMouseInput()
{
}

bool NodeMouseInput::Update(float dt, GameObject * object)
{
	updating = false;
	
	if (App->input->GetMouseButton(mouse_butt_id))
		updating = true;

	return updating;
}

void NodeMouseInput::Draw()
{
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
	

}
