#ifndef __NodeKeyInput_H__
#define __NodeKeyInput_H__

#include "Node.h"

class NodeKeyInput : public Node
{
public:
	NodeKeyInput(int id, const ImVec2& pos);
	~NodeKeyInput();

	bool Update(float dt, GameObject* object);
	void Draw();

private:

	int key_id = -1;
	char key = 'a';

};

#endif