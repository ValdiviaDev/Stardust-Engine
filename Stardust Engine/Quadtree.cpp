#include "Quadtree.h"
#include "GameObject.h"

#include <gl/GL.h>


QuadtreeNode::QuadtreeNode(AABB box, QuadtreeNode* parent): box(box), parent(parent)
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
				//Create children for the node
				CreateChilds();

				//Reorganize the GameObjects
				objects.push_back(go);
				ReorganizeObjects();
			}
		}
	}
	else {
		for (int i = 0; i < 4; ++i)
				childs[i]->Insert(go);
	}
}

void QuadtreeNode::Remove(GameObject* go)
{
	for (std::vector<GameObject*>::const_iterator it = objects.begin(); it != objects.end(); ++it)
		if ((*it) == go) {
			objects.erase(it);
			break;
		}

	//TODO: Look if we have to delete a node maybe?

	if (!IsLeaf()) {
		for (int i = 0; i < 4; ++i)
			childs[i]->Remove(go);
	}
}

void QuadtreeNode::CreateChilds()
{
	AABB child_box;
	float3 box_center = box.CenterPoint();
	float3 child_size = { box.Size().x / 2.0f, box.Size().y, box.Size().z / 2.0f };
	
	float3 child_center = { box_center.x - box.Size().x / 4.0f, box_center.y, box_center.z + box.Size().z / 4.0f };
	child_box.SetFromCenterAndSize(child_center, child_size);
	childs[0] = new QuadtreeNode(child_box, this); //Up Left
	
	child_center = { box_center.x + box.Size().x / 4.0f, box_center.y, box_center.z + box.Size().z / 4.0f };
	child_box.SetFromCenterAndSize(child_center, child_size);
	childs[1] = new QuadtreeNode(child_box, this); //Up Right
	
	child_center = { box_center.x - box.Size().x / 4.0f, box_center.y, box_center.z - box.Size().z / 4.0f };
	child_box.SetFromCenterAndSize(child_center, child_size);
	childs[2] = new QuadtreeNode(child_box, this); //Down Left
	
	child_center = { box_center.x + box.Size().x / 4.0f, box_center.y, box_center.z - box.Size().z / 4.0f };
	child_box.SetFromCenterAndSize(child_center, child_size);
	childs[3] = new QuadtreeNode(child_box, this); //Down Right
}

void QuadtreeNode::ReorganizeObjects()
{
	std::vector<GameObject*>::const_iterator it = objects.begin();

	while (it != objects.end()) {

		GameObject* obj = *it;

		uint boxes_intersection = 0;
		for (int i = 0; i < 4; ++i)
			if (childs[i]->box.Intersects((*it)->bounding_box))
				boxes_intersection++;

		//If two or more childs intersect keep the GO on the parent
		if (boxes_intersection > 1) {
			it++;
			//RELEASE(childs[0]);
			//RELEASE(childs[1]);
			//RELEASE(childs[2]);
			//RELEASE(childs[3]);
			//break;
		}
		//If they don't, put the GO on its appropiate child
		else {
			it = objects.erase(it);
				for (int i = 0; i < 4; ++i)
					childs[i]->Insert(obj);
		}

	}

	

	

}

bool QuadtreeNode::IsLeaf()
{
	if (childs[0] == nullptr)
		return true;
	else
		return false;
}

void QuadtreeNode::DebugDraw()
{
	for (uint i = 0; i < box.NumEdges(); i++)
	{
		glVertex3f(box.Edge(i).a.x, box.Edge(i).a.y, box.Edge(i).a.z);
		glVertex3f(box.Edge(i).b.x, box.Edge(i).b.y, box.Edge(i).b.z);
	}

	if (!IsLeaf())
		for (int i = 0; i < 4; ++i)
			childs[i]->DebugDraw();
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
		root->Remove(go);
}

void Quadtree::DebugDraw()
{
	glBegin(GL_LINES);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f); //Green

	if(root)
		root->DebugDraw();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnd();

}
