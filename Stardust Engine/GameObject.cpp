#include "GameObject.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "Quadtree.h"
#include "ConfigEditor.h"
#include "Parson/parson.h"

#include "imgui/imgui.h"
#include "Glew/include/glew.h"


GameObject::GameObject(GameObject* parent)
{
	this->parent = parent;
	if (parent != nullptr)
		parent->childs.push_back(this);
	
	//Always create transform
	CreateComponent(Comp_Transform);
	//bounding_box.SetNegativeInfinity();

	SetName("Default GO name");
	uuid = App->GenerateUUID();
	LOG("%i", uuid);
}


GameObject::~GameObject()
{
	for (int i = 0; i < GetNumChilds(); i++)
		RELEASE(childs[i]);
	childs.clear();

	RELEASE(transform);
	RELEASE(mesh);
	RELEASE(material);
	RELEASE(camera);

	parent = nullptr;
}

void GameObject::Update()
{

	for (uint i = 0; i < childs.size(); i++) {
		childs[i]->Update();
	}

	if (camera)
		camera->Update();
}

Component* GameObject::CreateComponent(ComponentType type, char* path, bool is_primitive)
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
			mesh = new ComponentMesh(this, path, is_primitive);
			component = mesh;
		}
		break;
	case Comp_Material:
		if (material == nullptr) {
			material = new ComponentMaterial(this);
			component = material;
		}
		break;
	case Comp_Camera:
		if (camera == nullptr) {
			camera = new ComponentCamera(this);
			component = camera;
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

bool GameObject::IsStatic() const
{
	return static_go;
}

void GameObject::SetStatic(bool staticGO)
{
	static_go = staticGO;
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

void GameObject::SetParent(GameObject* parent)
{
	if (this->parent)
		this->DeleteFromParentList();

	this->parent = parent;
}

void GameObject::DeleteFromParentList()
{
	if (parent) {
		for (std::vector<GameObject*>::const_iterator it = parent->childs.begin(); it < parent->childs.end(); it++)
			if ((*it) == this) { //TODO: probably wont have to change this
				parent->childs.erase(it);
				break;
			}
	}
}


void GameObject::GUIHierarchyPrint(int& i, bool& clicked) {
	//Push ID for each tree node
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

		//The GameObject that has begun to be dragged
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			GameObject* go = this;
			ImGui::SetDragDropPayload("Hierarchy", &go, sizeof(GameObject));
			
			ImGui::EndDragDropSource();
		}

		//Where the user is dropping the Source object
		if (ImGui::BeginDragDropTarget()) {
			
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Hierarchy")) {
				
				GameObject* source = *(GameObject**)payload->Data;

				if (!source->IsObjectInHierarchy(this, source)) { //Looks that the source GO isn't higher in the hierarchy than the target
					source->DeleteFromParentList();
					childs.push_back(source);
					source->parent = this;
				}
				else
					App->gui->AddLogToConsole("ERROR: You cannot make a GameObject the child of a GameObject lower on the hierarchy.");
			}

			ImGui::EndDragDropTarget();
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

	//GameObject main properties
	ImGui::Checkbox(" ", &active);
	ImGui::SameLine();
	ImGui::InputText("", name, IM_ARRAYSIZE(name));
	ImGui::SameLine();
	if (ImGui::Checkbox("Static", &static_go)) {
		if (static_go == true)
			App->scene->quadtree->Insert(this);
		else
			App->scene->quadtree->Remove(this);
	}
	
	//TODO Tag and Layer maybe?

	if (transform)
		transform->DrawInspector();

	if (mesh)
		mesh->DrawInspector();

	if (material)
		material->DrawInspector();

	if (camera)
		camera->DrawInspector();

}

bool GameObject::IsObjectInHierarchy(GameObject * target, GameObject* curr_node)
{
	if (target == curr_node)
		return true;

	for (int i = 0; i < curr_node->GetNumChilds(); ++i)
		if (IsObjectInHierarchy(target, curr_node->childs[i]))
			return true;

	return false;
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

void GameObject::UpdateBoundingBox() {

	bounding_box.SetNegativeInfinity();

	if (mesh) {

		bounding_box.Enclose((const math::float3*)mesh->m_info.vertex, mesh->m_info.num_vertex);
	}

	if (transform) {
		
		obb.SetFrom(bounding_box);
		obb.Transform(transform->GetGlobalMatrix());
		if (obb.IsFinite()) {
			bounding_box = obb.MinimalEnclosingAABB();
		}
	}

	for (uint i = 0; i < childs.size(); ++i){

		childs[i]->UpdateBoundingBox();
	}

}



void GameObject::DrawBoundingBox()
{
	glBegin(GL_LINES);
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);

	for (uint i = 0; i < bounding_box.NumEdges(); i++)
	{
		glVertex3f(bounding_box.Edge(i).a.x, bounding_box.Edge(i).a.y, bounding_box.Edge(i).a.z);
		glVertex3f(bounding_box.Edge(i).b.x, bounding_box.Edge(i).b.y, bounding_box.Edge(i).b.z);
	}
	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	for (uint i = 0; i < childs.size(); ++i)
	{
		childs[i]->DrawBoundingBox();
	}

}


void GameObject::Load(ConfigEditor* config)
{


}

void GameObject::Save(JSON_Array* go_array) const{

	LOG("Saving GameObject: %s", name);
	std::string uuid_s = std::to_string(uuid);

	
	//config->WriteUint("uuid", uuid);
	//config->AddArray("uuid");

	//config->array = json_value_get_array(config->root_value);
	

	JSON_Value* val = json_value_init_object();
	JSON_Object* obj = json_value_get_object(val);


	//Save GO info
	json_object_set_string(obj, "Name", name);
	json_object_set_number(obj, "UUID", uuid);
	json_object_set_boolean(obj, "Active", active);
	json_object_set_boolean(obj, "Static", static_go);
	if (parent)
		json_object_set_number(obj, "Parent UUID", parent->uuid);
	else
		json_object_set_number(obj, "Parent UUID", 0);



	//Save Components
	JSON_Value* value_comps = json_value_init_array();
	JSON_Array* array_comps = json_value_get_array(value_comps);
	
	if (transform)
		transform->Save(array_comps);
	if (mesh)
		mesh->Save(array_comps);
	if (material)
		material->Save(array_comps);
	if (camera)
		camera->Save(array_comps);



	json_object_set_value(obj, "Components", value_comps);
	
	json_array_append_value(go_array, val);


}