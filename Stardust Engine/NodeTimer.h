#ifndef __NodeTimer_H__
#define __NodeTimer_H__

#include "Node.h"

class NodeTimer : public Node
{
public:
	NodeTimer(int id, const ImVec2& pos, uint timer_num);
	~NodeTimer();

	bool Update(float dt, std::vector<GameObject*> BB_objects, uint num_comp_graph);
	void Draw(std::vector<GameObject*> BB_objects);

	void Save(JSON_Object* obj) const;
	void Load(JSON_Object* obj);

private:
	bool reset = false;
	bool repeat_action = false;

	float time = 0.0f;

	uint timer_num = 0;

};

#endif