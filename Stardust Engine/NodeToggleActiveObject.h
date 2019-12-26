#ifndef __NodeToggleActiveObject_H__
#define __NodeToggleActiveObject_H__

#include "Node.h"

enum ActiveSetTo {
	Toggle,
	True,
	False

};

class NodeToggleActiveObject : public Node
{
public:
	NodeToggleActiveObject(int id, const ImVec2& pos);
	~NodeToggleActiveObject();

	bool Update(float dt, std::vector<GameObject*> BB_objects, uint num_comp_graph);
	void Draw(std::vector<GameObject*> BB_objects);

	void Save(JSON_Object* obj) const;
	void Load(JSON_Object* obj);

private:
	const char* curr_active_set_str = "Toggle";
	ActiveSetTo active_set = Toggle;
};

#endif