#ifndef __NodeLogToConsole_H__
#define __NodeLogToConsole_H__

#include "Node.h"

class NodeLogToConsole : public Node
{
public:
	NodeLogToConsole(int id, const ImVec2& pos);
	~NodeLogToConsole();

	bool Update(float dt, std::vector<GameObject*> BB_objects, uint num_comp_graph);
	void Draw(std::vector<GameObject*> BB_objects);

	void Save(JSON_Object* obj) const;
	void Load(JSON_Object* obj);

private:
	char log[150] = "";

};

#endif