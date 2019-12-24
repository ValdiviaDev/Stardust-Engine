#include "Application.h"
#include "NodeLogToConsole.h"
#include "ModuleGui.h"


NodeLogToConsole::NodeLogToConsole(int id, const ImVec2& pos) : Node(id, "Action: LogToConsole", pos, 1, 1, Node_Type_Action, Func_LogToConsole)
{
}


NodeLogToConsole::~NodeLogToConsole()
{
}

bool NodeLogToConsole::Update(float dt, std::vector<GameObject*> BB_objects)
{
	node_state = Node_State_Updating;

	App->gui->AddLogToConsole(log);

	return true;
}

void NodeLogToConsole::Draw(std::vector<GameObject*> BB_objects)
{
	ImGui::Text("Log to print");
	ImGui::InputText("", log, IM_ARRAYSIZE(log));
}


void NodeLogToConsole::Save() const {

}

void NodeLogToConsole::Load() {

}