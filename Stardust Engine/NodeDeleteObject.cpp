#include "Application.h"
#include "NodeDeleteObject.h"
#include "ModuleScene.h"
#include "ModuleGui.h"



NodeDeleteObject::NodeDeleteObject(int id, const ImVec2& pos) : Node(id, "Action: DeleteObject", pos, 1, 1, Node_Type_Action, Func_DelObject)
{
}


NodeDeleteObject::~NodeDeleteObject()
{
}

bool NodeDeleteObject::Update(float dt, std::vector<GameObject*> BB_objects)
{
	node_state = Node_State_Idle;
	GameObject* to_delete = nullptr;

	//If reference gets deleted, send error
	if (obj_indx >= BB_objects.size())
		node_state = Node_State_Error;
	else
		to_delete = BB_objects[obj_indx];

	if (to_delete) {
		App->scene->GO_to_delete = to_delete;
		App->scene->want_to_delete_go = true;

		node_state = Node_State_Updating;
	}
	else
		App->gui->AddLogToConsole("ERROR: You are trying to delete a GameObject that is already deleted");

	return true;
}

void NodeDeleteObject::Draw(std::vector<GameObject*> BB_objects)
{
	//GameObject reference
	DrawObjectsInstance(BB_objects);

}

void NodeDeleteObject::Save(JSON_Object* obj) const {

	json_object_set_boolean(obj, "using this", obj_using_this);
	json_object_set_number(obj, "id using", obj_indx);

}

void NodeDeleteObject::Load(JSON_Object* obj) {

	obj_using_this = json_object_get_boolean(obj, "using this");
	obj_indx = json_object_get_number(obj, "id using");
}