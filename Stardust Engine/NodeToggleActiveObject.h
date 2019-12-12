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

	bool Update(float dt, std::vector<GameObject*> BB_objects);
	void Draw(std::vector<GameObject*> BB_objects);

private:
	const char* curr_active_set_str = "Toggle";
	ActiveSetTo active_set = Toggle;
};

#endif