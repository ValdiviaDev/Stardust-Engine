#ifndef __NodeInstantiateObject_H__
#define __NodeInstantiateObject_H__

#include "Node.h"

class NodeInstantiateObject : public Node
{
public:
	NodeInstantiateObject(int id, const ImVec2& pos);
	~NodeInstantiateObject();
	
	bool Update(float dt, std::vector<GameObject*> BB_objects);
	void Draw(std::vector<GameObject*> BB_objects);

private:


};

#endif