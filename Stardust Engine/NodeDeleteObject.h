#ifndef __NodeDeleteObject_H__
#define __NodeDeleteObject_H__

#include "Node.h"

class NodeDeleteObject : public Node
{
public:
	NodeDeleteObject(int id, const ImVec2& pos);
	~NodeDeleteObject();

	bool Update(float dt, std::vector<GameObject*> BB_objects, uint num_comp_graph);
	void Draw(std::vector<GameObject*> BB_objects);

	void Save(JSON_Object* obj) const;
	void Load(JSON_Object* obj);

private:

};

#endif