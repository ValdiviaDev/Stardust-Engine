#ifndef __NodeLogToConsole_H__
#define __NodeLogToConsole_H__

#include "Node.h"

class NodeLogToConsole : public Node
{
public:
	NodeLogToConsole(int id, const ImVec2& pos);
	~NodeLogToConsole();

	bool Update(float dt, std::vector<GameObject*> BB_objects);
	void Draw(std::vector<GameObject*> BB_objects);

private:
	char log[150] = "";

};

#endif