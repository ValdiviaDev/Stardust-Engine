#ifndef __Node_H__
#define __Node_H__

#include "NodeGraph.h"

enum NodeType {
	Node_Type_Default,
	Node_Type_Event,
	Node_Type_Function
};

enum NodeSubType {
	Node_Default,
	Node_KeyInput,

};

class Node
{
public:

	Node(int id, const char* name, const ImVec2& pos, int inputs_count, int outputs_count, NodeType type, NodeSubType sub_type);

	virtual ~Node();

	ImVec2 GetInputSlotPos(int slot_no) const;
	ImVec2 GetOutputSlotPos(int slot_no) const;

	void SetInputsCount(int num);
	void SetOutputsCount(int num);

	virtual void Draw() {}
	virtual bool Update(float dt) { return false; }

public:

	int     ID;
	char    Name[32];
	ImVec2  Pos, Size;
	float   Value;
	ImVec4  Color;
	int     InputsCount, OutputsCount;
	std::vector<Node*>inputs;
	std::vector<Node*>outputs;

	NodeType type;
	NodeSubType sub_type;

	bool updating = false;
};

#endif