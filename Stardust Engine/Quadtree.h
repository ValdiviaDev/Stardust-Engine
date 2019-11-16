#ifndef __QuadTree__H__
#define __Quadtree__H__

#define QUADTREE_MAX_ITEMS 2

#include "MathGeoLib/include/MathGeoLib.h"
#include <vector>
class GameObject;

using namespace std;

class QuadtreeNode
{
public:
	QuadtreeNode(AABB box, QuadtreeNode* parent);
	~QuadtreeNode();

	void Insert(GameObject* go);
	void Remove(GameObject* go);
	template<typename TYPE>
	inline void Intersect(vector<GameObject*>& objects, const TYPE& primitive);

	void CreateChilds();
	void ReorganizeObjects();

	bool IsLeaf();
	void DebugDraw();

private:
	math::AABB box;
	vector<GameObject*> objects;
	QuadtreeNode* parent = nullptr;
	QuadtreeNode* childs[4];
};

class Quadtree
{
public:
	Quadtree();
	~Quadtree();

	void Create(math::AABB limits);
	void Clear();
	void Insert(GameObject* go);
	void Remove(GameObject* go);
	template<typename TYPE>
	inline void Intersect(vector<GameObject*>& objects, const TYPE& primitive);

	void DebugDraw();

private:
	QuadtreeNode* root = nullptr;

};

#endif

template<typename TYPE>
inline void Quadtree::Intersect(vector<GameObject*>& objects, const TYPE& primitive)
{
	if (root)
		root->Intersect(objects, primitive);
}

template<typename TYPE>
inline void QuadtreeNode::Intersect(vector<GameObject*>& objects, const TYPE& primitive)
{
	if (primitive.Intersects(box))
	{
		for (std::vector<GameObject*>::const_iterator it = this->objects.begin(); it != this->objects.end(); ++it)
		{
			if (primitive.Intersects((*it)->bounding_box))
				objects.push_back(*it);

		}
		for (int i = 0; i < 4; ++i)
			if (childs[i] != nullptr) childs[i]->Intersect(objects, primitive);

	}
}
