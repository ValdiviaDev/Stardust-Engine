#ifndef __NodeMouseMotion_H__
#define __NodeMouseMotion_H__

#include "Node.h"

class NodeMouseMove : public Node
{
public:
	NodeMouseMove(int id, const ImVec2& pos);
	~NodeMouseMove();

	bool Update(float dt, std::vector<GameObject*> BB_objects);
	void Draw(std::vector<GameObject*> BB_objects);

	void Save(JSON_Object* obj) const;
	void Load(JSON_Object* obj);

private:


};

#endif