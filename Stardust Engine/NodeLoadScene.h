#ifndef __NodeLoadScene_H__
#define __NodeLoadScene_H__

#include "Node.h"

class NodeLoadScene : public Node
{
public:
	NodeLoadScene(int id, const ImVec2& pos);
	~NodeLoadScene();

	bool Update(float dt, std::vector<GameObject*> BB_objects);
	void Draw(std::vector<GameObject*> BB_objects);

	void Save(JSON_Object* obj) const;
	void Load(JSON_Object* obj);

private:
	char scene_to_load[100] = "";

};

#endif