#ifndef __NodeKeyInput_H__
#define __NodeKeyInput_H__

#include "Node.h"

enum KEY_STATE;

class NodeKeyInput : public Node
{
public:
	NodeKeyInput(int id, const ImVec2& pos);
	~NodeKeyInput();

	bool Update(float dt, std::vector<GameObject*> BB_objects, uint num_comp_graph);
	void Draw(std::vector<GameObject*> BB_objects);

	void Save(JSON_Object* obj) const;
	void Load(JSON_Object* obj);

private:

	int key_id;
	char key = 'a';
	KEY_STATE key_state;
	const char* curr_state_str = "Key Repeat";

};

#endif