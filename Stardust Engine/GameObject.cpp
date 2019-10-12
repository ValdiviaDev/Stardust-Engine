#include "GameObject.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "imgui/imgui.h"

GameObject::GameObject(GameObject* parent)
{
	this->parent = parent;
	if (parent != nullptr)
		parent->childs.push_back(this);
	
	//Always create transform
	CreateComponent(Comp_Transform);

	name = "Default GO name";
}


GameObject::~GameObject()
{
	//Delete Components
	for (int i = 0; i < components.size(); ++i)
		RELEASE(components[i]);
}

void GameObject::Update()
{
}

Component* GameObject::CreateComponent(ComponentType type, const char* path, int num_mesh)
{
	Component* component = nullptr;

	switch (type) {
	case Comp_Transform:
		transform = new ComponentTransform(this);
		component = transform;
		break;
	case Comp_Mesh:
		mesh = new ComponentMesh(this, path, num_mesh);
		component = mesh;
		break;
	case Comp_Material:
		material = new ComponentMaterial(this);
		component = material;
		break;
	case Comp_Default:
		component = new Component(this);
		break;
	}
	components.push_back(component);

	return component;
}

bool GameObject::GetActive() const
{
	return active;
}

void GameObject::SetActive(bool active)
{
	this->active = active;
}


void GameObject::SetName(const char* new_name) {
	name = new_name;
	LOG("name = %s", name);
}

uint GameObject::GetNumChilds() const
{
	return childs.size();
}

GameObject* GameObject::GetAChild(uint i) 
{
	return childs[i];

}

GameObject* GameObject::GetParent() {
	return parent;
}



void GameObject::GUIHierarchyPrint(int& i) {

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

	if (GetNumChilds() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;
/*
	if (ImGui::TreeNodeEx(name, flags)) {

		LOG("AAAAAAAAAAAAAAA %s", name);
		for (int i = 0; i < GetNumChilds(); i++) {
			GetAChild(i)->GUIHierarchyPrint();
		}

	}
*/
	bool tree_open = false;

	if (ImGui::TreeNodeEx((void*)(intptr_t)i, flags, name))
		tree_open = true;


	if (tree_open) {

		LOG("AAAAAAAAAAAAAAA %s", name);
		for (int j = 0; j < GetNumChilds(); j++) {
			ImGui::PushID(i);
			i++;
			GetAChild(j)->GUIHierarchyPrint(i);
			ImGui::PopID();
		}
	}



}