#ifndef __NodeGraph_H__
#define __NodeGraph_H__


#include "Globals.h"
#include <string>
#include "imgui/imgui.h"
#include <vector>

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
	std::vector<Node*>inputs;
	std::vector<Node*>outputs;

	Node(int id, const char* name, const ImVec2& pos, float value, const ImVec4& color, int inputs_count, int outputs_count) {
		ID = id; strncpy(Name, name, 31); Name[31] = 0; Pos = pos; Value = value; 
		Color = color; InputsCount = inputs_count;	OutputsCount = outputs_count;
	}

	~Node() {

		//LOOK IF WE NEED TO CLEAR THIS OR IT CLEARS WITH THE ~NodeGraph()
	}

	ImVec2 GetInputSlotPos(int slot_no) const { return ImVec2(Pos.x, Pos.y + Size.y * ((float)slot_no + 1) / ((float)InputsCount + 1)); }
	ImVec2 GetOutputSlotPos(int slot_no) const { return ImVec2(Pos.x + Size.x, Pos.y + Size.y * ((float)slot_no + 1) / ((float)OutputsCount + 1)); }
};
struct NodeLink
{
	int     InputIdx, InputSlot, OutputIdx, OutputSlot;

	NodeLink(int input_idx, int input_slot, int output_idx, int output_slot) {  //id of node input, slot number, id of node output, slot number 
		InputIdx = input_idx; InputSlot = input_slot; OutputIdx = output_idx; OutputSlot = output_slot;
	}
};




class NodeGraph {

public:

	NodeGraph();
	~NodeGraph();
	void Draw();

	Node* AddNode(const char* name, const ImVec2& pos, int inputs_count, int outputs_count, float value = 0.0f, const ImVec4& color = ImColor(255, 100, 100));
	void AddLink(int input_idx, int input_slot, int output_idx, int output_slot);
	void DeleteLink(int node_id, int slot_num);
	void DeleteNode(Node* node);
	Node* GetNodeByID(int ID);
private:

	
	bool show_grid = true;
	std::vector<Node*> nodes;
	std::vector<NodeLink> links;
	
	ImVec2 scrolling = ImVec2(0.0f, 0.0f);
	
	int last_node_id = -1;

	int input_id_clicked = -1;
	int input_slot_clicked = -1;
	int output_id_clicked = -1;
	int output_slot_clicked = -1;
};



#endif
