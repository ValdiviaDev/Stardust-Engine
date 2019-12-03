#include "Application.h"
#include "ComponentMesh.h"
#include "Globals.h"
#include "imgui/imgui.h"
#include "MeshImporter.h"
#include "GameObject.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "ModuleGui.h"
#include "ModuleScene.h"

#include "Glew/include/glew.h"
#include <gl/GL.h>

#include "ResourceMesh.h"

ComponentMesh::ComponentMesh(GameObject* parent) : Component(parent)
{
	type = Comp_Mesh;


}

ComponentMesh::~ComponentMesh()
{
	//Unload Resource
	if (uuid_mesh != 0) {
		ResourceMesh* res = (ResourceMesh*)App->resources->Get(uuid_mesh);
		if(res)
			res->UnloadToMemory();
	}

}

bool ComponentMesh::IsPrimitive() const
{
	if(is_primitive == PRIMITIVE_NONE)
		return false;
	
	return true;
}

void ComponentMesh::SetPrimitive(PrimitiveType type)
{
	is_primitive = type;
}

void ComponentMesh::DrawInspector() {

	if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::Checkbox("Mesh Active", &active);

		//Change mesh via drag and drop
		const char* drag_msg = "";
		if (HasMesh())
			drag_msg = "Drag mesh here";
		else
			drag_msg = "Drag new mesh here";

		ImGui::Button(drag_msg);
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets")) {
				UID uuid = *(UID*)payload->Data;
				if (App->resources->Get(uuid) != nullptr)
					App->scene->AssignMeshToGameObject(uuid);
				else
					App->gui->AddLogToConsole("This mesh isn't loaded as a resource");

			}
			ImGui::EndDragDropTarget();
		}

		ImGui::Separator();

		ImGui::Text("Mesh path: ");
		ImGui::SameLine();

		if(path)
			ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.0f, 1.0f), path);
		else
			ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "null");

		ImGui::Separator();

		if (!IsPrimitive()) {
		ImGui::Text("Debug Options");

			ImGui::Checkbox("Vertex Normals", &debug_v_norm);
			ImGui::SameLine();
			ImGui::Checkbox("Face Normals", &debug_f_norm);
		}

		ImGui::Separator();
		
		
		if (HasMesh()) {
			ResourceMesh* res = (ResourceMesh*)App->resources->Get(uuid_mesh);
			if (res != nullptr) {
				ImGui::Text("Num of GameObjects that use this resource:");
				ImGui::SameLine();
				ImGui::TextColored({ 255,255,0,255 }, "%u", res->CountReferences());
				ImGui::Separator();
			}
		}

	}

}

void ComponentMesh::SetPath(const char* path)
{
	this->path = path;
}

bool ComponentMesh::HasMesh() const
{
	return uuid_mesh != 0;
}


void ComponentMesh::Save(JSON_Array* comp_array) const {

	JSON_Value* value = json_value_init_object();
	JSON_Object* obj = json_value_get_object(value);

	json_object_set_number(obj, "Component Type", type);
	json_object_set_number(obj, "UUID Mesh", uuid_mesh);
	json_object_set_string(obj, "path", path);
	json_object_set_number(obj, "is primitive", is_primitive);
	json_object_set_boolean(obj, "Active", active);

	json_array_append_value(comp_array, value);

}


void ComponentMesh::Load(JSON_Object* comp_obj) {

	int aux = json_object_get_number(comp_obj, "is primitive");
	is_primitive = (PrimitiveType)aux;
	uuid_mesh = json_object_get_number(comp_obj, "UUID Mesh");
	active = json_object_get_boolean(comp_obj, "Active");
	const char* p = json_object_get_string(comp_obj, "path");
	char* ap = (char*)p;
	std::string fullpath;
	std::string file;
	App->fs->SplitFilePath(p, &fullpath, &file);

	switch (is_primitive) {
	case PRIMITIVE_NONE:
		if (HasMesh()) {
			ResourceMesh* res = (ResourceMesh*)App->resources->Get(uuid_mesh);
			if (res)
				res->LoadToMemory();
#ifndef GAME_MODE
			else {
				string log = "GameObject " + (string)gameObject->GetName() + ": Charging a mesh component with no mesh";
				App->gui->AddLogToConsole(log.c_str());
			}
#endif
		}

		break;

	case PRIMITIVE_CUBE:
	case PRIMITIVE_SPHERE: 
	case PRIMITIVE_PLANE:
	{
		ResourceMesh* res = App->resources->GetPrimitive(is_primitive);
		if (res)
			res->LoadToMemory();
		uuid_mesh = res->GetUID();
	}
		break;
	}

	gameObject->UpdateBoundingBox();
}