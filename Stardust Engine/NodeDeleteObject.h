#ifndef __NodeDeleteObject_H__
#define __NodeDeleteObject_H__

#include "Node.h"

class NodeDeleteObject : public Node
{
public:
	NodeDeleteObject(int id, const ImVec2& pos);
	~NodeDeleteObject();

	bool Update(float dt, std::vector<GameObject*> BB_objects);
	void Draw(std::vector<GameObject*> BB_objects);

	void Save() const;
	void Load();

private:

};

#endif