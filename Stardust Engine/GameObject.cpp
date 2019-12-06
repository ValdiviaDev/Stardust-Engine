#include "Application.h"
#include "GameObject.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "ComponentGraphScript.h"
#include "Quadtree.h"
#include "ConfigEditor.h"
#include "Parson/parson.h"
#include "ResourceMesh.h"
#include "ModuleResourceManager.h"
#include "ModuleGui.h"
#include "ModuleCamera3D.h"
#include "ModuleScene.h"
#include "imgui/imgui.h"
#include "Glew/include/glew.h"


GameObject::GameObject(GameObject* parent)
{
	this->parent = parent;
	if (parent != nullptr)
		parent->childs.push_back(this);
	
	//Always create transform
	CreateComponent(Comp_Transform);

	SetName("Default GO name");
	uuid = App->GenerateUUID();
}


GameObject::~GameObject()
{
	for (int i = 0; i < GetNumChilds(); i++)
		RELEASE(childs[i]);
	childs.clear();


	for (int i = components.size() - 1; i >= 0; --i) {
	
		RELEASE(components[i]);
	}
	components.clear();

	//RELEASE(transform);
	//RELEASE(mesh);
	//RELEASE(material);
	//RELEASE(camera);

	parent = nullptr;
}

void GameObject::Update(float dt)
{
	//Child GameObjects update
	for (uint i = 0; i < childs.size(); i++)
		childs[i]->Update(dt);

	//Components update
	for (int i = 0; i < components.size(); ++i)
		components[i]->Update(dt);
}

Component* GameObject::CreateComponent(ComponentType type)
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
		if (GetComponent(Comp_Mesh) == nullptr)
			component = new ComponentMesh(this);
		else
			return nullptr;
		break;
	case Comp_Material:
		if (GetComponent(Comp_Material) == nullptr)
			component = new ComponentMaterial(this);
		else
			return nullptr;
		break;
	case Comp_Camera:
		if (GetComponent(Comp_Camera) == nullptr)
			component = new ComponentCamera(this);
		else
			return nullptr;
		break;
	case Comp_Graph_Script:
		component = new ComponentGraphScript(this);
		break;
	case Comp_Default:
		return nullptr;
		break;
	}

	if (component)
		components.push_back(component);

	return component;
}

Component* GameObject::GetComponent(ComponentType type) const
{
	for (int i = 0; i < components.size(); ++i) {
		if (type == components[i]->GetType())
			return components[i];
	}

	return nullptr;
}

void GameObject::DeleteFromComponentList(Component* comp)
{
	for (std::vector<Component*>::const_iterator it = components.begin(); it < components.end(); it++) {
		if ((*it) == comp) {

			components.erase(it);

			break;

		}
	}

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
	if (!charge_static)
		static_go = staticGO;
	
	charge_static = false;
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

GameObject* GameObject::GetChild(uint i) const
{
	return childs[i];

}

GameObject* GameObject::GetParent() const {
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
			if ((*it) == this) {
				parent->childs.erase(it);
				break;
			}
	}
}


void GameObject::GUIHierarchyPrint(int& i, bool& clicked) {
	//Push ID for each tree node
	ImGui::PushID(i);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen;

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
					source->transform->SetTransformFromParent();
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
		if (static_go == true) {
			App->scene->quadtree->Insert(this);
			App->scene->static_objects.push_back(this);
			App->scene->CheckIfRebuildQuadtree(this);
		}
		else {
			App->scene->quadtree->Remove(this);
			App->scene->EraseObjFromStatic(this);
		}
	}

	if (transform)
		transform->DrawInspector();

	//TODO: Change this structure probably
	if (GetComponent(Comp_Mesh))
		GetComponent(Comp_Mesh)->DrawInspector();

	if (GetComponent(Comp_Material))
		GetComponent(Comp_Material)->DrawInspector();

	if (GetComponent(Comp_Camera))
		GetComponent(Comp_Camera)->DrawInspector();

	//Draw all component scripts
	for (int i = 0; i < components.size(); ++i) {
		if (components[i]->GetType() == Comp_Graph_Script)
			components[i]->DrawInspector();
	}

	//Add component TODO
	ImGui::Separator();
	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("New Component");
	
	if (ImGui::BeginPopupContextItem("New Component"))
	{
		if (GetComponent(Comp_Mesh) == nullptr) {
			if (ImGui::MenuItem("Mesh"))
				CreateComponent(Comp_Mesh);
		}

		if (GetComponent(Comp_Material) == nullptr) {
			if (ImGui::MenuItem("Material"))
				CreateComponent(Comp_Material);
		}

		if (GetComponent(Comp_Camera) == nullptr) {
			if (ImGui::MenuItem("Camera"))
				CreateComponent(Comp_Camera);
		}

		if (ImGui::MenuItem("Graph Script")) //There can be multiple graph scripts in one GameObject
			CreateComponent(Comp_Graph_Script);

		ImGui::EndPopup();
	}

}

bool GameObject::IsObjectInHierarchy(GameObject* target, GameObject* curr_node)
{
	if (target == curr_node)
		return true;

	for (int i = 0; i < curr_node->GetNumChilds(); ++i)
		if (IsObjectInHierarchy(target, curr_node->childs[i]))
			return true;

	return false;
}



void GameObject::CenterCameraOnGO() const {

	float3 dir_vector = -transform->GetGlobalPos() + App->camera->engine_cam->frustum.pos;
	dir_vector.Normalize();

	

	float rad = bounding_box.MinimalEnclosingSphere().r;
	float dist = math::Abs(App->camera->engine_cam->frustum.orthographicWidth / App->camera->engine_cam->frustum.orthographicHeight * rad / math::Sin(App->camera->engine_cam->frustum.verticalFov / 2));


	App->camera->Look(bounding_box.Centroid() + dir_vector * dist, bounding_box.Centroid());

}

void GameObject::UpdateBoundingBox() {

	bounding_box.SetNegativeInfinity();

	ComponentMesh* mesh = (ComponentMesh*)GetComponent(Comp_Mesh);
	if (mesh && mesh->uuid_mesh != 0) {
		ResourceMesh* res_mesh = (ResourceMesh*)App->resources->Get(mesh->uuid_mesh);
		bounding_box.Enclose((const math::float3*)res_mesh->vertex, res_mesh->num_vertex);
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

}


void GameObject::Load(JSON_Object* object)
{
	SetName(json_object_get_string(object, "Name"));
	uuid = json_object_get_number(object, "UUID");
	active = json_object_get_boolean(object, "Active");
	static_go = json_object_get_boolean(object, "Static");
	charge_static = true;

	JSON_Array* array_comps = json_object_get_array(object, "Components");
	JSON_Object* it;

	for (uint i = 0; i < json_array_get_count(array_comps); i++) {
		
		it = json_array_get_object(array_comps, i);
		

		int comp_type = json_object_get_number(it, "Component Type");

		switch (comp_type) {

		case ComponentType::Comp_Default:
			break;

		case ComponentType::Comp_Transform:

			if(transform == nullptr)
				CreateComponent((ComponentType)comp_type);
			transform->Load(it);
			break;

		case ComponentType::Comp_Mesh:
		{
			ComponentMesh* mesh = (ComponentMesh*)CreateComponent((ComponentType)comp_type);
			mesh->Load(it);
			break;
		}
		case ComponentType::Comp_Material:
		{
			ComponentMaterial* mat = (ComponentMaterial*)CreateComponent((ComponentType)comp_type);
			mat->Load(it);
			break;
		}
		case ComponentType::Comp_Camera:
		{
			ComponentCamera* cam = (ComponentCamera*)CreateComponent((ComponentType)comp_type);
			cam->Load(it);
			break;
		}
		}
	}

}

//Used for saving scene, so we save all GOs in array
void GameObject::Save(JSON_Array* go_array) const {

	LOG("Saving GameObject: %s", name);
	std::string uuid_s = std::to_string(uuid);


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

	if (GetComponent(Comp_Mesh)) 
	{
		ComponentMesh* mesh = (ComponentMesh*)GetComponent(Comp_Mesh);
		mesh->Save(array_comps);
	}
	if (GetComponent(Comp_Material))
	{
		ComponentMaterial* mat = (ComponentMaterial*)GetComponent(Comp_Material);
		mat->Save(array_comps);
	}
	if (GetComponent(Comp_Camera))
	{
		ComponentCamera* cam = (ComponentCamera*)GetComponent(Comp_Camera);
		cam->Save(array_comps);
	}


	json_object_set_value(obj, "Components", value_comps);
	
	json_array_append_value(go_array, val);


}


