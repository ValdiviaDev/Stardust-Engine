#ifndef __NodeMouseInput_H__
#define __NodeMouseInput_H__

#include "Node.h"
class NodeMouseInput : public Node
{
public:
	NodeMouseInput(int id, const ImVec2& pos);
	~NodeMouseInput();

	bool Update(float dt, GameObject* object);
	void Draw();

private:

	int mouse_butt_id = 1;
	const char* curr_butt = "Left";
};

#endif