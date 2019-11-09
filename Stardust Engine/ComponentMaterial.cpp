#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleImport.h"
#include "imgui/imgui.h"

ComponentMaterial::ComponentMaterial(GameObject* parent) : Component(parent)
{
	type = Comp_Material;
}


ComponentMaterial::~ComponentMaterial()
{
}

void ComponentMaterial::AssignTexture(const char* path)
{
	//Import texture with DevIL
	bool charged = App->importer->ImportTexture((char*)path, tex_id, tex_width, tex_height);

	if (charged) {
		has_tex = true;
		this->tex_path = path;
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


void ComponentMaterial::DrawInspector() {

	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
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


void ComponentMaterial::Save(JSON_Array* comp_array) {

	JSON_Value* value = json_value_init_object();
	JSON_Object* obj = json_value_get_object(value);

	json_object_set_number(obj, "Component Type", type);

	json_object_set_string(obj, "path", tex_path);

}