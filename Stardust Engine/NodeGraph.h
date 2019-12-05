#ifndef __NodeGraph_H__
#define __NodeGraph_H__


#include "Globals.h"
#include <string>
#include "imgui/imgui.h"


#define NODE_WINDOW_PADDING ImVec2(8.0f, 8.0f)
#define NODE_SLOT_RADIUS 4.0f


static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }


// Dummy
struct Node
{
	int     ID;
	char    Name[32];
	ImVec2  Pos, Size;
	float   Value;
	ImVec4  Color;
	int     InputsCount, OutputsCount;

	Node(int id, const char* name, const ImVec2& pos, float value, const ImVec4& color, int inputs_count, int outputs_count) {
		ID = id; strncpy(Name, name, 31); Name[31] = 0; Pos = pos; Value = value; Color = color; InputsCount = inputs_count;
		OutputsCount = outputs_count;
	}

	ImVec2 GetInputSlotPos(int slot_no) const { return ImVec2(Pos.x, Pos.y + Size.y * ((float)slot_no + 1) / ((float)InputsCount + 1)); }
	ImVec2 GetOutputSlotPos(int slot_no) const { return ImVec2(Pos.x + Size.x, Pos.y + Size.y * ((float)slot_no + 1) / ((float)OutputsCount + 1)); }
};
struct NodeLink
{
	int     InputIdx, InputSlot, OutputIdx, OutputSlot;

	NodeLink(int input_idx, int input_slot, int output_idx, int output_slot) {
		InputIdx = input_idx; InputSlot = input_slot; OutputIdx = output_idx; OutputSlot = output_slot;
	}
};




class NodeGraph {

public:

	NodeGraph();

	void Draw();


private:

	
	bool show_grid = true;
	ImVector<Node> nodes;
	ImVector<NodeLink> links;
	
	ImVec2 scrolling = ImVec2(0.0f, 0.0f);
	
	

};



#endif
