#include "ComponentMaterial.h"
#include "Application.h"
#include "imgui/imgui.h"
#include "GameObject.h"

#include "ResourceTexture.h"

ComponentMaterial::ComponentMaterial(GameObject* parent) : Component(parent)
{
	type = Comp_Material;
}


ComponentMaterial::~ComponentMaterial()
{
	if (uuid_mat != 0) {
		ResourceTexture* res = (ResourceTexture*)App->resources->Get(uuid_mat);
		if (res)
			res->UnloadToMemory();
	}

	if (gameObject)
		gameObject->material = nullptr;
}


void ComponentMaterial::AssignTextureLib(const char * path)
{
	if (path) {
		/*//The path in this case is the name file (UUID maybe)
		bool charged = App->mat_import->LoadMaterial(path, this);

		if (charged)
			has_tex = true;
		//Maybe we can put the assets path here? or maybe not
		*/
		string file(path);
		if (file.find(".")) {
			string aux1, aux2;
			App->fs->SplitFilePath(path, &aux1, &file, &aux2);
			file = file.substr(0, file.find_last_of("."));
			//has_tex = App->mat_import->LoadMaterial(file.c_str(), this);
		}
		else {}
			//has_tex = App->mat_import->LoadMaterial(path, this);

	}
}

uint ComponentMaterial::GetTexId() const
{
	return tex_id;
}

bool ComponentMaterial::GetIfTex() const
{
	return has_tex;
}

void ComponentMaterial::SetPath(const char * path)
{
	strcpy(tex_path, path);
}


void ComponentMaterial::DrawInspector() {

	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("UUID: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.0f, 1.0f), std::to_string(uuid_mat).c_str());

		ImGui::Text("Texture path: ");
		ImGui::SameLine();

		if (has_tex) {
			ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.0f, 1.0f), tex_path);

			ImGui::Text("Texture size: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.0f, 1.0f), "%i x %i", tex_width, tex_height);

		}
		else
			ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "null");

		ImGui::Separator();

		ImGui::Text("Debug Options");

		ImGui::Checkbox("Draw Texture", &debug_tex_draw);
		ImGui::SameLine();
		ImGui::Checkbox("Draw Checkers", &debug_checkers);

		if (has_tex)
			ImGui::Image((ImTextureID)tex_id, { 250,250 });

	}

}


void ComponentMaterial::Save(JSON_Array* comp_array) const {

	JSON_Value* value = json_value_init_object();
	JSON_Object* obj = json_value_get_object(value);

	json_object_set_number(obj, "Component Type", type);
	json_object_set_number(obj, "UUID Material", uuid_mat);
	json_object_set_string(obj, "path", tex_path);

	json_array_append_value(comp_array, value);
}

void ComponentMaterial::Load(JSON_Object* comp_obj) {

	
	uuid_mat = json_object_get_number(comp_obj, "UUID Material");
	AssignTextureLib(json_object_get_string(comp_obj, "path"));

}