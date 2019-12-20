#include "Application.h"
#include "NodeInstantiateObject.h"
#include "ModuleScene.h"
#include "ModuleResourceManager.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ResourceMesh.h"



NodeInstantiateObject::NodeInstantiateObject(int id, const ImVec2& pos) : Node(id, "Event: InstantiateObject", pos, 1, 1, Node_Type_Action, Func_InstObject)
{
}


NodeInstantiateObject::~NodeInstantiateObject()
{
}

bool NodeInstantiateObject::Update(float dt, std::vector<GameObject*> BB_objects)
{
	node_state = Node_State_Idle;
	GameObject* to_instance = nullptr;
	GameObject* ref = nullptr;

	if (dt == 0.0f) {
		node_state = Node_State_ToUpdate;
		return true; //Show that node is updating, but nothing happens, because the simulation is paused
	}

	//If reference gets deleted, send error
	if (inst_indx >= BB_objects.size() || inst_indx <= 0 || obj_indx >= BB_objects.size())
		node_state = Node_State_Error;
	else {
		to_instance = BB_objects[inst_indx];
		ref = BB_objects[obj_indx];
	}

	//Instance the gameobject
	if (to_instance) {

		GameObject* new_inst = App->scene->CreateGameObject(App->scene->GetRootGameObject());
		new_inst->SetName(to_instance->GetName());

		//Transform
		new_inst->transform->SetPosition(pos_to_inst);
		new_inst->transform->SetRotation(rot_to_inst);
		new_inst->transform->SetScale(to_instance->transform->GetScale());

		for (int i = 0; i < to_instance->GetNumComp(); ++i) {
			if (to_instance->GetComponentByIndex(i)->GetType() == Comp_Mesh)
				CopyMesh(new_inst, (ComponentMesh*)to_instance->GetComponentByIndex(i));

			//TODO: material
			//TODO: camera
			//TODO: graph scripts

		}

		node_state = Node_State_Updating;
	}

	return true;
}

void NodeInstantiateObject::Draw(std::vector<GameObject*> BB_objects)
{
	ImGui::TextColored({ 255,255,0,255 }, "Object to instance");
	ObjectToInstanceDropDown(BB_objects);

	ImGui::TextColored({ 255,255,0,255 }, "Transform");

	ImGui::Checkbox("Get transform from reference", &get_ref_trans);

	if (!get_ref_trans) {

		ImGui::DragFloat3("Position", &pos_to_inst[0], 0.1f);
		ImGui::DragFloat3("Rotation", &rot_to_inst[0], 0.1f);
	}

	else  {
		//GameObject reference
		ImGui::Text("Object to get transform from");
		DrawObjectsInstance(BB_objects);
	}

}

void NodeInstantiateObject::CopyMesh(GameObject* new_inst, ComponentMesh* to_instance)
{
	ComponentMesh* mesh = (ComponentMesh*)new_inst->CreateComponent(Comp_Mesh);
	
	if (!mesh->IsPrimitive()) {
		//Load new mesh resource
		ResourceMesh* res_mesh = nullptr;
		res_mesh = (ResourceMesh*)App->resources->Get(to_instance->uuid_mesh);
		if (res_mesh)
			res_mesh->LoadToMemory();
	
		mesh->uuid_mesh = to_instance->uuid_mesh;
		mesh->SetPath(res_mesh->GetFile());
		new_inst->UpdateBoundingBox();
	}
}

void NodeInstantiateObject::ObjectToInstanceDropDown(std::vector<GameObject*> BB_objects)
{
	const char* object_name;

	//Check if reference has been deleted
	if (inst_indx < BB_objects.size() && inst_indx > 0)
		object_name = BB_objects[inst_indx]->GetName();
	else
		object_name = "null";

	if (ImGui::BeginCombo(" ", object_name)) {
		for (int i = 1; i < BB_objects.size(); ++i) {
			if (ImGui::Selectable(BB_objects[i]->GetName())) {
				inst_indx = i;
				pos_to_inst = BB_objects[i]->transform->GetPosition();
				rot_to_inst = BB_objects[i]->transform->GetRotation();
			}
		}
		ImGui::EndCombo();
	}
}
