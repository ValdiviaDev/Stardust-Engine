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
	for (int i = 0; i < GetNumChilds(); i++)
		RELEASE(childs[i]);
	childs.clear();

	RELEASE(transform);
	RELEASE(mesh);
	RELEASE(material);

	parent = nullptr;
}

void GameObject::Update()
{
}

Component* GameObject::CreateComponent(ComponentType type, char* path, int num_mesh)
{
	Component* component = nullptr;

	switch (type) {
	case Comp_Transform:
		if (transform == nullptr) {
			transform = new ComponentTransform(this);
			component = transform;
		}
		break;
	case Comp_Mesh:
		if (mesh == nullptr) {
			mesh = new ComponentMesh(this, path, num_mesh);
			component = mesh;
		}
		break;
	case Comp_Material:
		if (material == nullptr) {
			material = new ComponentMaterial(this);
			component = material;
		}
		break;
	case Comp_Default:
		return nullptr;
		break;
	}

	return component;
}

bool GameObject::IsActive() const
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

const char * GameObject::GetName() const
{
	return name;
}

uint GameObject::GetNumChilds() const
{
	return childs.size();
}

GameObject* GameObject::GetChild(uint i) 
{
	return childs[i];

}

GameObject* GameObject::GetParent() {
	return parent;
}

void GameObject::DeleteFromParentList()
{
	if (parent) {
		for (std::vector<GameObject*>::const_iterator it = parent->childs.begin(); it < parent->childs.end(); it++)
			if ((*it)->GetName() == name) { //Change name for Uuid later on development
				parent->childs.erase(it);
				break;
			}
	}
}


void GameObject::GUIHierarchyPrint(int& i) {

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

	if (GetNumChilds() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;
/*
	if (ImGui::TreeNodeEx(name, flags)) {

		LOG("AAAAAAAAAAAAAAA %s", name);
		for (int i = 0; i < GetNumChilds(); i++) {
			GetChild(i)->GUIHierarchyPrint();
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
			GetChild(j)->GUIHierarchyPrint(i);
			ImGui::PopID();
		}
	}

	//ImGui::TreeNodePop(); //???

}


void GameObject::DrawComponentsInspector() {

	if (transform) {
		ImGui::PushID(0);
		transform->DrawInspector();
		ImGui::PopID();
	}

	if (mesh) {
		ImGui::PushID(1);
		mesh->DrawInspector();
		ImGui::PopID();
	}

	if (material) {
		ImGui::PushID(2);
		material->DrawInspector();
		ImGui::PopID();
	}
}