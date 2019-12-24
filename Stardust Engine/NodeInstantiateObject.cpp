#include "Application.h"
#include "NodeInstantiateObject.h"
#include "ModuleScene.h"
#include "ModuleResourceManager.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "ComponentGraphScript.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"
#include "ResourceGraphScript.h"



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

		//Set position
		if (get_ref_trans) {
			pos_to_inst = ref->transform->GetPosition();
			rot_to_inst = ref->transform->GetRotation();
		}

		InstanceObject(to_instance);
		
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

void NodeInstantiateObject::InstanceObject(GameObject* to_instance)
{
	GameObject* new_inst = App->scene->CreateGameObject(App->scene->GetRootGameObject());
	new_inst->SetName(to_instance->GetName());

	//Transform
	new_inst->transform->SetPosition(pos_to_inst);
	new_inst->transform->SetRotation(rot_to_inst);
	new_inst->transform->SetScale(to_instance->transform->GetScale());

	//Components
	for (int i = 0; i < to_instance->GetNumComp(); ++i) {
		switch (to_instance->GetComponentByIndex(i)->GetType()) {
		case Comp_Mesh:
			CopyMesh(new_inst, (ComponentMesh*)to_instance->GetComponentByIndex(i));
			break;
		case Comp_Material:
			CopyMaterial(new_inst, (ComponentMaterial*)to_instance->GetComponentByIndex(i));
			break;
		case Comp_Camera:
			CopyCamera(new_inst, (ComponentCamera*)to_instance->GetComponentByIndex(i));
			break;
		case Comp_Graph_Script:
			CopyCompGraph(new_inst, (ComponentGraphScript*)to_instance->GetComponentByIndex(i));
			break;
		}
	}

	//Active the object if instance is inactive
	new_inst->SetActive(true);
}

void NodeInstantiateObject::CopyMesh(GameObject* new_inst, ComponentMesh* to_instance)
{
	ComponentMesh* mesh = (ComponentMesh*)new_inst->CreateComponent(Comp_Mesh);
	
	if (!mesh->IsPrimitive()) {
		//Load new mesh resource
		ResourceMesh* res_mesh = nullptr;
		res_mesh = (ResourceMesh*)App->resources->Get(to_instance->uuid_mesh);
		if (res_mesh) {
			res_mesh->LoadToMemory();
			mesh->uuid_mesh = to_instance->uuid_mesh;
			mesh->SetPath(res_mesh->GetFile());
			new_inst->UpdateBoundingBox();
		}
	}
}

void NodeInstantiateObject::CopyMaterial(GameObject* new_inst, ComponentMaterial* to_instance)
{
	ComponentMaterial* mat = (ComponentMaterial*)new_inst->CreateComponent(Comp_Material);

	//Load new resource
	ResourceTexture* res_tex = nullptr;
	res_tex = (ResourceTexture*)App->resources->Get(to_instance->uuid_mat);
	if (res_tex) {
		res_tex->LoadToMemory();
		//Assign the tex to the material
		mat->uuid_mat = res_tex->GetUID();
		mat->SetPath(res_tex->GetFile());
	}

}

void NodeInstantiateObject::CopyCamera(GameObject* new_inst, ComponentCamera* to_instance)
{
	ComponentCamera* cam = (ComponentCamera*)new_inst->CreateComponent(Comp_Camera);
	cam->SetFOV(to_instance->GetFOV());
	cam->SetNearPlane(to_instance->GetNearPlane());
	cam->SetFarPlane(to_instance->GetFarPlane());

}

void NodeInstantiateObject::CopyCompGraph(GameObject* new_inst, ComponentGraphScript* to_instance)
{
	ComponentGraphScript* gs = (ComponentGraphScript*)new_inst->CreateComponent(Comp_Graph_Script);

	ResourceGraphScript* res_gs = nullptr;
	res_gs = (ResourceGraphScript*)App->resources->Get(to_instance->uuid_script);
	if (res_gs) {
		res_gs->LoadToMemory();
		gs->uuid_script = to_instance->uuid_script;

		//Copy the blackboard
		std::vector<GameObject*> blackboard = to_instance->GetBlackboard();
		for (int i = 1; i < blackboard.size(); ++i)
			gs->ForceAddReferenceToBlackboard(blackboard[i]);

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
				//Set the transform of the object to instance
				pos_to_inst = BB_objects[i]->transform->GetPosition();
				rot_to_inst = BB_objects[i]->transform->GetRotation();
			}
		}
		ImGui::EndCombo();
	}
}

void NodeInstantiateObject::Save(JSON_Object* obj) const {


	json_object_set_boolean(obj, "get ref trans", get_ref_trans);

	json_object_set_number(obj, "posX", pos_to_inst.x);
	json_object_set_number(obj, "posY", pos_to_inst.y);
	json_object_set_number(obj, "posZ", pos_to_inst.z);

	json_object_set_number(obj, "rotX", rot_to_inst.x);
	json_object_set_number(obj, "rotY", rot_to_inst.y);
	json_object_set_number(obj, "rotZ", rot_to_inst.z);

	json_object_set_number(obj, "inst indx", inst_indx);

	json_object_set_boolean(obj, "using this", obj_using_this);
	json_object_set_number(obj, "id using", obj_indx);
}


void NodeInstantiateObject::Load(JSON_Object* obj) {


	get_ref_trans= json_object_get_boolean(obj, "get ref trans");

	pos_to_inst.x = json_object_get_number(obj, "posX");
	pos_to_inst.y = json_object_get_number(obj, "posY");
	pos_to_inst.z = json_object_get_number(obj, "posZ");

	rot_to_inst.x = json_object_get_number(obj, "rotX");
	rot_to_inst.y = json_object_get_number(obj, "rotY");
	rot_to_inst.z = json_object_get_number(obj, "rotZ");

	inst_indx = json_object_get_number(obj, "inst indx");

	obj_using_this = json_object_get_boolean(obj, "using this");
	obj_indx = json_object_get_number(obj, "id using");

}