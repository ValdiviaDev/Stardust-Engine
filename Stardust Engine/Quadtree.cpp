#include "Quadtree.h"
#include "GameObject.h"



QuadtreeNode::QuadtreeNode(AABB box, QuadtreeNode* parent)
{
	for (int i = 0; i < 4; ++i)
		childs[i] = nullptr;
}

QuadtreeNode::~QuadtreeNode()
{
	for (int i = 0; i < 4; ++i)
			RELEASE(childs[i]);
}

void QuadtreeNode::Insert(GameObject * go)
{
	//TODO: Maybe this structure has bugs and have to change
	if (IsLeaf()) { 
		if (go->bounding_box.Intersects(box)) {
			if (objects.size() < QUADTREE_MAX_ITEMS) {

				objects.push_back(go);
			}
			else {

				//TODO: Create children for the node

				//TODO: Reorganize the node


			}
		}
	}
	else {
		for (int i = 0; i < 4; ++i)
				childs[i]->Insert(go);
	}
}

void QuadtreeNode::Erase(GameObject* go)
{
	for (std::vector<GameObject*>::const_iterator it = objects.begin(); it != objects.end(); ++it)
		if ((*it) == go)
			objects.erase(it);

	if (!IsLeaf()) {
		for (int i = 0; i < 4; ++i)
			childs[i]->Erase(go);

	}

}

bool QuadtreeNode::IsLeaf()
{
	if (childs[0] == nullptr)
		return true;
	else
		return false;
}



Quadtree::Quadtree()
{
}


Quadtree::~Quadtree()
{
	Clear();
}

void Quadtree::Create(math::AABB limits)
{
	Clear();
	root = new QuadtreeNode(limits, nullptr);
}

void Quadtree::Clear()
{
	RELEASE(root);
}

void Quadtree::Insert(GameObject* go) // enclose
{
	if (root)
		root->Insert(go);
}

void Quadtree::Remove(GameObject * go)
{
	if (root)
		root->Insert(go);
}
