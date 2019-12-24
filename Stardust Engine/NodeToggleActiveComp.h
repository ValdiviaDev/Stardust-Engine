#ifndef __NodeToggleActiveComp_H__
#define __NodeToggleActiveComp_H__

#include "Node.h"

enum ActiveSetTo;
enum ComponentType;

class NodeToggleActiveComp : public Node
{
public:
	NodeToggleActiveComp(int id, const ImVec2& pos);
	~NodeToggleActiveComp();

	bool Update(float dt, std::vector<GameObject*> BB_objects);
	void Draw(std::vector<GameObject*> BB_objects);

	void Save() const;
	void Load();

private:
	const char* curr_active_set_str = "Toggle";
	ActiveSetTo active_set;
	const char* comp_str = "Mesh";
	ComponentType comp_type;

};

#endif