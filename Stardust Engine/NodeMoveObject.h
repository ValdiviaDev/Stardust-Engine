#ifndef __NodeMoveObject_H__
#define __NodeMoveObject_H__

#include "Node.h"

class NodeMoveObject : public Node
{
public:
	NodeMoveObject(int id, const ImVec2& pos);
	~NodeMoveObject();

	bool Update(float dt, std::vector<GameObject*> BB_objects, uint num_comp_graph);
	void Draw(std::vector<GameObject*> BB_objects);

	void Save(JSON_Object* obj) const;
	void Load(JSON_Object* obj);

private:

	int direction[3];
	const char* dir_str = "Forward";
	float velocity = 0.0f;
	bool this_go = true;
	bool move_global = true;
	bool dont_change_dir_local = false;
};

#endif