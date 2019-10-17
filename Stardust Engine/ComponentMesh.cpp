#include "ComponentMesh.h"
#include "Globals.h"
#include "imgui/imgui.h"

ComponentMesh::ComponentMesh(GameObject* parent, char* path, int num_mesh, bool is_primitive) 
	: Component(parent), path(path), num_mesh(num_mesh), is_primitive(is_primitive)
{
	type = Comp_Mesh;
	m_info = geo_info();

	if(!is_primitive)
		AssignMesh(path);
}

ComponentMesh::~ComponentMesh()
{
	if (!is_primitive) {
		RELEASE_ARRAY(m_info.index);
		RELEASE_ARRAY(m_info.vertex);
		RELEASE_ARRAY(m_info.normal);
		RELEASE_ARRAY(m_info.uv);
		RELEASE_ARRAY(m_info.color);
	}
}

void ComponentMesh::AssignMesh(char* path)
{
	//Path
	//char* path_source = "Assets/Meshes/";
	//char full_path[100];
	//strcpy(full_path, path_source);
	//strcat(full_path, path);

	bool charged = false;

	//Import mesh and bind buffers
	charged = App->importer->ImportMesh(path, m_info, this->gameObject, num_mesh);
	if (charged) {
		App->importer->BindBuffers(m_info);
		this->path = path;
	}
}

geo_info ComponentMesh::GetInfo() const
{
	
	return m_info;
}

bool ComponentMesh::IsPrimitive() const
{
	return is_primitive;
}

void ComponentMesh::DrawInspector() {

	if (ImGui::CollapsingHeader("Mesh")) {
		ImGui::Text("Mesh path: ");
		ImGui::SameLine();

		if(path)
			ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.0f, 1.0f), path);
		else
			ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "null");

		ImGui::Separator();

		if (!is_primitive) {
		ImGui::Text("Debug Options");

			ImGui::Checkbox("Vertex Normals", &debug_v_norm);
			ImGui::SameLine();
			ImGui::Checkbox("Face Normals", &debug_f_norm);
		}

	}

}

void ComponentMesh::FillPrimitiveDrawInfo(geo_info info)
{
	m_info = info;
	//m_info.num_vertex = info.num_vertex;
	//m_info.num_index = info.num_index;
	//m_info.vertex = new float[m_info.num_vertex];
	//for (uint i = 0; i < m_info.num_vertex; ++i)
	//	memcpy(&m_info.vertex[i], &info.vertex[i], sizeof(float)  * m_info.num_vertex);
	//
	//
	//m_info.index = new uint[m_info.num_index];
	//for (uint i = 0; i < m_info.num_index; ++i)
	//	memcpy(&m_info.index[i], &info.index[i], sizeof(uint) * m_info.num_index);


	App->importer->BindBuffersPrimitive(m_info);

}
