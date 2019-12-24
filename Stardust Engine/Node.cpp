#include "Node.h"
#include "GameObject.h"

Node::Node(int id, const char * name, const ImVec2 & pos, int inputs_count, int outputs_count, NodeType type, NodeFunction function)
{
	ID = id;
	strncpy(Name, name, 31);
	Name[31] = 0;
	Pos = pos;
	InputsCount = inputs_count;
	OutputsCount = outputs_count;
	this->type = type;
	this->function = function;
}

Node::~Node()
{
	//LOOK IF WE NEED TO CLEAR THIS OR IT CLEARS WITH THE ~NodeGraph()
}

ImVec2 Node::GetInputSlotPos(int slot_no) const
{
	return ImVec2(Pos.x, Pos.y + Size.y * ((float)slot_no + 1) / ((float)InputsCount + 1));
}

ImVec2 Node::GetOutputSlotPos(int slot_no) const
{
	return ImVec2(Pos.x + Size.x, Pos.y + Size.y * ((float)slot_no + 1) / ((float)OutputsCount + 1));
}

void Node::SetInputsCount(int num)
{
	InputsCount = num;
}

void Node::SetOutputsCount(int num)
{
	OutputsCount = num;
}

void Node::DrawObjectsInstance(std::vector<GameObject*> BB_objects)
{
	if (ImGui::Checkbox("This object", &obj_using_this)) {
		if (obj_using_this)
			obj_indx = 0;
	}

	if (!obj_using_this) {
		const char* object_name;

		//Check if reference has been deleted
		if (obj_indx < BB_objects.size())
			object_name = BB_objects[obj_indx]->GetName();
		else
			object_name = "null";

		if (ImGui::BeginCombo("GameObject reference", object_name)) {
			for (int i = 1; i < BB_objects.size(); ++i) {
				if (ImGui::Selectable(BB_objects[i]->GetName()))
					obj_indx = i;
			}

			ImGui::EndCombo();
		}
	}
}


void Node::Save() const {

}

void Node::Load() {

}