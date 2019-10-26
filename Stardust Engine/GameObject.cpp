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

	SetName("Default GO name");

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

Component* GameObject::CreateComponent(ComponentType type, char* path, int num_mesh, bool is_primitive)
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
			mesh = new ComponentMesh(this, path, num_mesh, is_primitive);
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
	
	strcpy_s(name, 100, new_name);
	//name = (char)new_name;
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


void GameObject::GUIHierarchyPrint(int& i, bool& clicked) {
	//Pop ID for each tree node
	ImGui::PushID(i);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;

	if (focused)
		flags |= ImGuiTreeNodeFlags_Selected;

	if (GetNumChilds() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

	

	if (ImGui::TreeNodeEx((void*)(intptr_t)i, flags, name)) {
		
		//Set focused to print Inspector
		if (ImGui::IsItemClicked() && !clicked) {
			LOG("%s node clicked", name);
			clicked = true;
			App->scene->GetRootGameObject()->focused = false;
			App->scene->FocusGameObject(this, App->scene->GetRootGameObject());
		}
		
		
		//Print each child of the gameobject
		


		for (int j = 0; j < GetNumChilds(); j++) {
			i++;
			GetChild(j)->GUIHierarchyPrint(i,clicked);
			
		}
		
		ImGui::TreePop();
	}
	//Set focused to print Inspector
	if (ImGui::IsItemClicked() && !clicked) {
		LOG("%s node clicked", name);
		clicked = true;
		App->scene->GetRootGameObject()->focused = false;
		App->scene->FocusGameObject(this, App->scene->GetRootGameObject());
	}
	ImGui::PopID();
}


void GameObject::DrawComponentsInspector() {



	
	if (ImGui::CollapsingHeader("GameObject")) {
		ImGui::PushID(0);
		ImGui::Text("Name:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.0f, 1.0f), name);
		ImGui::PopID();
	}

	if (transform) {
		ImGui::PushID(1);
		transform->DrawInspector();
		ImGui::PopID();
	}

	if (mesh) {
		ImGui::PushID(2);
		mesh->DrawInspector();
		ImGui::PopID();
	}

	if (material) {
		ImGui::PushID(3);
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
			
			max_x = info.vertex[i];
			
		}
		if (info.vertex[i] < min_x) {
			
			min_x = info.vertex[i];

		}

		if (info.vertex[i + 1] > max_y) {
			
			max_y = info.vertex[i + 1];

		}
		if (info.vertex[i + 1] < min_y) {
			
			min_y = info.vertex[i + 1];

		}

		if (info.vertex[i + 2] > max_z) {
			
				max_z = info.vertex[i + 2];

		}
		if (info.vertex[i + 2] < min_z) {
			
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
	dist = 1 + z_length + ( (dist / 2) / math::Tan(DEGTORAD * 30));
	LOG("formula = %f", dist);


	float offset_y = 0;
	offset_y = min_y;

	if (transform) {
		
		float3 pos = transform->GetGlobalPos();
		float3 scale = transform->GetGlobalScale();
		App->camera->Look(vec3(pos.x + (min_x + (max_x - min_x)/2) * scale.x, pos.y + (min_y + (max_y - min_y) / 2) * scale.y , (pos.z + dist) * scale.z), vec3(pos.x + (min_x + (max_x - min_x) / 2)*scale.x, pos.y + (min_y + (max_y - min_y) / 2) * scale.y, pos.z * scale.z));
	}
	else {
		App->camera->Look(vec3(0, 0, dist), vec3(0, 0, 0));
	}

	
	

}