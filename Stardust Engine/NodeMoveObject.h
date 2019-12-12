#ifndef __NodeMoveObject_H__
#define __NodeMoveObject_H__

#include "Node.h"

class NodeMoveObject : public Node
{
public:
	NodeMoveObject(int id, const ImVec2& pos);
	~NodeMoveObject();

	bool Update(float dt, std::vector<GameObject*> BB_objects);
	void Draw(std::vector<GameObject*> BB_objects);

private:

	int direction[3];
	float velocity = 0.0f;
	bool this_go = true;
	GameObject* go;

};

#endif