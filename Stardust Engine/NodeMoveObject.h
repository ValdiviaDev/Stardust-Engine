#ifndef __NodeMoveObject_H__
#define __NodeMoveObject_H__

#include "Node.h"

class NodeMoveObject : public Node
{
public:
	NodeMoveObject(int id, const ImVec2& pos);
	~NodeMoveObject();

	bool Update(float dt, GameObject* object);
	void Draw();

private:

	int direction[3];
	float velocity = 0.0f;

};

#endif