#ifndef __NodeKeyInput_H__
#define __NodeKeyInput_H__

#include "Node.h"

enum KEY_STATE;

class NodeKeyInput : public Node
{
public:
	NodeKeyInput(int id, const ImVec2& pos);
	~NodeKeyInput();

	bool Update(float dt, GameObject* object);
	void Draw();

private:

	int key_id;
	char key = 'a';
	KEY_STATE key_state;
	const char* curr_state_str = "Key Repeat";

};

#endif