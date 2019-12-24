#ifndef __Node_H__
#define __Node_H__

#include "NodeGraph.h"

enum NodeType {
	Node_Type_Default,
	Node_Type_Event,
	Node_Type_Action
};

enum NodeFunction {
	Func_Default,
	Func_KeyInput,
	Func_MouseInput,
	Func_MouseMove,
	Func_MoveObject,
	Func_RotateObject,
	Func_ToggleActiveObject,
	Func_ToggleActiveComp,
	Func_LogToConsole,
	Func_InstObject,
	Func_DelObject,
	Func_LoadScene,
	Func_Timer
};

enum NodeState {
	Node_State_Updating,
	Node_State_ToUpdate,
	Node_State_Error,
	Node_State_Idle
};

class Node
{
public:

	Node(int id, const char* name, const ImVec2& pos, int inputs_count, int outputs_count, NodeType type, NodeFunction function);

	virtual ~Node();

	ImVec2 GetInputSlotPos(int slot_no) const;
	ImVec2 GetOutputSlotPos(int slot_no) const;

	void SetInputsCount(int num);
	void SetOutputsCount(int num);

	virtual bool Update(float dt, std::vector<GameObject*> BB_objects) { return false; }
	virtual void Draw(std::vector<GameObject*> BB_objects) {}

	virtual void Save(JSON_Object* obj) const;
	virtual void Load(JSON_Object* obj);

	void DrawObjectsInstance(std::vector<GameObject*> BB_objects);

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
	NodeFunction function;

	NodeState node_state = Node_State_Idle;
	uint obj_indx = 0;
	bool obj_using_this = true;

};

#endif