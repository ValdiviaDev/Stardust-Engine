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
			if ((*it)->GetName() == name) { //Change name for Uuid later on development TODO
				parent->childs.erase(it);
				break;
			}
	}
}


void GameObject::GUIHierarchyPrint(int& i) {
	//Pop ID for each tree node
	ImGui::PushID(i);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

	if (GetNumChilds() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

	if (ImGui::TreeNodeEx((void*)(intptr_t)i, flags, name)) {
		//Print each child of the gameobject
		for (int j = 0; j < GetNumChilds(); j++) {
			i++;
			GetChild(j)->GUIHierarchyPrint(i);
		}
		ImGui::TreePop();
	}

	ImGui::PopID();
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



void GameObject::CenterCameraOnGO() const {

	geo_info info = mesh->GetInfo();
	int num_vertex = info.num_vertex;

	float max_x = 0, min_x = 0, max_y = 0, min_y = 0, max_z = 0, min_z = 0;

	for (int i = 0; i < num_vertex; i+=3) {
		if (i == 0)
		{
			max_x = min_x = info.vertex[0];
			max_y = min_y = info.vertex[1];
			max_z = min_z = info.vertex[2];
		}

		if (info.vertex[i] > max_x) {
			LOG("MaxX %f > %f", info.vertex[i], max_x)
			max_x = info.vertex[i];
			
		}
		if (info.vertex[i] < min_x) {
			LOG("minX %f < %f", info.vertex[i], min_x)
			min_x = info.vertex[i];

		}

		if (info.vertex[i + 1] > max_y) {
			LOG("MaxY %f > %f", info.vertex[i+1], max_y)
			max_y = info.vertex[i + 1];

		}
		if (info.vertex[i + 1] < min_y) {
			LOG("minY %f < %f", info.vertex[i+1], min_y)
			min_y = info.vertex[i + 1];

		}

		if (info.vertex[i + 2] > max_z) {
			LOG("MaxZ %f > %f", info.vertex[i + 2], max_z)
				max_z = info.vertex[i + 2];

		}
		if (info.vertex[i + 2] < min_z) {
			LOG("minZ %f > %f", info.vertex[i + 2], min_z)
				min_z = info.vertex[i + 2];

		}
	}

	

	float dist;
	if (max_x - min_x > max_y - min_y)
		dist = max_x - min_x;
	else
		dist = max_y - min_y;

	float z_length = max_z - min_z;

	LOG("MAX(%f, %f, %f), min(%f, %f, %f). Dist = %f, Z = %f", max_x, max_y, max_z, min_x, min_y, min_z, dist, z_length);

	//d = (s/2) / tan(a/2)
	dist = z_length + ( (dist / 2) / math::Tan(DEGTORAD * 30));
	LOG("formula = %f", dist);

	if (transform) {
		float3 pos = transform->GetPosition();
		float3 scale = transform->GetScale();
		App->camera->Look(vec3(pos.x * scale.x, pos.y * scale.y, (pos.z + dist)*scale.z), vec3(pos.x, pos.y, pos.z));
	}
	else {
		App->camera->Look(vec3(0, 0, dist), vec3(0, 0, 0));
	}

	
	

}