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

class Node;
enum NodeSubType;

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
	void Update(float dt);

	Node* AddNode(const char* name, const ImVec2& pos, int inputs_count, int outputs_count, NodeSubType node_sub_type, float value = 0.0f, const ImVec4& color = ImColor(255, 100, 100));
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

	//TODO: Delete later
	bool inited = false;

};

#endif