#ifndef __NodeTimer_H__
#define __NodeTimer_H__

#include "Node.h"

class NodeTimer : public Node
{
public:
	NodeTimer(int id, const ImVec2& pos);
	~NodeTimer();

	bool Update(float dt, std::vector<GameObject*> BB_objects);
	void Draw(std::vector<GameObject*> BB_objects);

private:

};

#endif