#include "Application.h"
#include "ComponentMaterial.h"
#include "Application.h"
#include "imgui/imgui.h"
#include "GameObject.h"
#include "ModuleResourceManager.h"
#include "ResourceTexture.h"
#include "ModuleGui.h"
#include "ModuleScene.h"

ComponentMaterial::ComponentMaterial(GameObject* parent) : Component(parent)
{
	type = Comp_Material;
}


ComponentMaterial::~ComponentMaterial()
{
	//Unload Resource
	if (uuid_mat != 0) {
		ResourceTexture* res = (ResourceTexture*)App->resources->Get(uuid_mat);
		if (res)
			res->UnloadToMemory();
	}

}


bool ComponentMaterial::HasTex() const
{
	return uuid_mat != 0;
}

void ComponentMaterial::SetPath(const char * path)
{
	strcpy(tex_path, path);
}


void ComponentMaterial::DrawInspector() {

	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::Checkbox("Material Active", &active);

		//Drag and drop for the texture
		const char* drag_msg = "";
		if (HasTex())
			drag_msg = "Drag tex here";
		else
			drag_msg = "Drag new tex here";
		ImGui::Button(drag_msg);
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets_Tex")) {
				UID uuid = *(UID*)payload->Data;
				if (App->resources->Get(uuid) != nullptr)
					App->scene->AssignTexToGameObject(uuid);
				else
					App->gui->AddLogToConsole("This texture isn't loaded as a resource");

			}
			ImGui::EndDragDropTarget();
		}
		
		ImGui::Separator();

		ImGui::Text("UUID: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.0f, 1.0f), std::to_string(uuid_mat).c_str());

		ImGui::Text("Texture path: ");
		ImGui::SameLine();
		
		ResourceTexture* r_tex = nullptr;
		if (HasTex()) {
			r_tex = (ResourceTexture*)App->resources->Get(uuid_mat);

			ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.0f, 1.0f), tex_path);

			ImGui::Text("Texture size: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.0f, 1.0f), "%i x %i", r_tex->tex_width, r_tex->tex_height);

		}
		else
			ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "null");

		ImGui::Separator();

		ImGui::Text("Debug Options");
		ImGui::Checkbox("Draw Checkers", &debug_checkers);

		if (HasTex()) {
			ImGui::Image((ImTextureID)r_tex->tex_id, { 250,250 });

			ImGui::Separator();

			if (r_tex) {
				ImGui::Text("Num of GameObjects that use this resource:");
				ImGui::SameLine();
				ImGui::TextColored({ 255,255,0,255 }, "%u", r_tex->CountReferences());
				ImGui::Separator();
			}

		}

	}

}


void ComponentMaterial::Save(JSON_Array* comp_array) const {

	JSON_Value* value = json_value_init_object();
	JSON_Object* obj = json_value_get_object(value);

	json_object_set_number(obj, "Component Type", type);
	json_object_set_number(obj, "UUID Material", uuid_mat);
	json_object_set_string(obj, "path", tex_path);
	json_object_set_boolean(obj, "Active", active);

	json_array_append_value(comp_array, value);
}

void ComponentMaterial::Load(JSON_Object* comp_obj) {

	uuid_mat = json_object_get_number(comp_obj, "UUID Material");
	active = json_object_get_boolean(comp_obj, "Active");

	if (uuid_mat != 0) {
		ResourceTexture* r_tex = (ResourceTexture*)App->resources->Get(uuid_mat);
		r_tex->LoadToMemory();
	}
#ifndef GAME_MODE
	else {
		string log = "GameObject " + (string)gameObject->GetName() + ": Charging a material with no texture";

		App->gui->AddLogToConsole(log.c_str());
	}
#endif

}