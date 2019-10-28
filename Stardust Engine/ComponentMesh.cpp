#include "ComponentMesh.h"
#include "Globals.h"
#include "imgui/imgui.h"
#include "MeshImporter.h"

ComponentMesh::ComponentMesh(GameObject* parent, char* path, bool is_primitive) 
	: Component(parent), path(path), is_primitive(is_primitive)
{
	type = Comp_Mesh;
	m_info = geo_info();

	if(path)
		AssignMesh(path);
}

ComponentMesh::~ComponentMesh()
{

	RELEASE_ARRAY(m_info.index);
	RELEASE_ARRAY(m_info.vertex);
	RELEASE_ARRAY(m_info.normal);
	RELEASE_ARRAY(m_info.uv);
	RELEASE_ARRAY(m_info.color);

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
	charged = App->importer->ImportMesh(path, m_info, this->gameObject);
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

void ComponentMesh::FillPrimitiveDrawInfo(par_shapes_mesh* shape)
{

	m_info.num_vertex = (uint)shape->npoints;
	m_info.vertex = new float[m_info.num_vertex * 3];
	memcpy(m_info.vertex, shape->points, sizeof(float) * m_info.num_vertex * 3);

	m_info.num_index = (uint)shape->ntriangles;
	m_info.index = new uint[m_info.num_index * 3];
	memcpy(m_info.index, shape->triangles, sizeof(PAR_SHAPES_T) * m_info.num_index * 3);

	App->importer->BindBuffersPrimitive(m_info);

}

void ComponentMesh::ChargePreLoadedMesh()
{

	string out_f;
	App->mesh_import->Import("Assets/Meshes/BakerHouse.fbx", "Assets/Meshes/BakerHouse.fbx", out_f);
	App->mesh_import->LoadMesh("own_file_baker.stdtmesh", m_info);

	App->importer->BindBuffers(m_info);
}

void ComponentMesh::SetPath(const char* path)
{
	this->path = path;
}
