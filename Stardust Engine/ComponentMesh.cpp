#include "ComponentMesh.h"
#include "Globals.h"
#include "imgui/imgui.h"
#include "MeshImporter.h"
#include "GameObject.h"

ComponentMesh::ComponentMesh(GameObject* parent, PrimitiveType primitive) 
	: Component(parent), is_primitive(primitive)
{
	type = Comp_Mesh;
	m_info = geo_info();


}

ComponentMesh::~ComponentMesh()
{
	RELEASE_ARRAY(m_info.index);
	RELEASE_ARRAY(m_info.vertex);
	RELEASE_ARRAY(m_info.normal);
	RELEASE_ARRAY(m_info.uv);
	RELEASE_ARRAY(m_info.color);

	if (gameObject)
		gameObject->mesh = nullptr;
}

void ComponentMesh::AssignMesh(const char* path)
{
	bool charged = false;

	//Import mesh and bind buffers
	charged = App->importer->ImportMesh(path, m_info, this->gameObject);
	if (charged) {
		App->importer->BindBuffers(m_info);
		this->path = path;
	}

	gameObject->UpdateBoundingBox();
}

geo_info ComponentMesh::GetInfo() const
{
	
	return m_info;
}

bool ComponentMesh::IsPrimitive() const
{
	if(is_primitive == PRIMITIVE_NONE)
		return false;
	
	return true;
}

void ComponentMesh::DrawInspector() {

	if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {
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

	gameObject->UpdateBoundingBox();

}

bool ComponentMesh::LoadMesh(string name)
{
	string path = name + "." + MESH_EXTENSION;
	bool ret = App->mesh_import->LoadMesh(path.c_str(), m_info);
	
	App->importer->BindBuffers(m_info);
	gameObject->UpdateBoundingBox();

	return ret;
}

void ComponentMesh::SetPath(const char* path)
{
	this->path = path;
}


void ComponentMesh::Save(JSON_Array* comp_array) {


	JSON_Value* value = json_value_init_object();
	JSON_Object* obj = json_value_get_object(value);

	json_object_set_number(obj, "Component Type", type);
	json_object_set_number(obj, "UUID Mesh", uuid_mesh);
	json_object_set_string(obj, "path", path);
	json_object_set_number(obj, "is primitive", is_primitive);
	

	json_array_append_value(comp_array, value);

}


void ComponentMesh::Load(JSON_Object* comp_obj) {

	int aux = json_object_get_number(comp_obj, "is primitive");
	is_primitive = (PrimitiveType)aux;
	uuid_mesh = json_object_get_number(comp_obj, "UUID Mesh");
	const char* p = json_object_get_string(comp_obj, "path");
	char* ap = (char*)p;
	std::string fullpath;
	std::string file;
	App->fs->SplitFilePath(p, &fullpath, &file);

	switch (is_primitive) {
	case PRIMITIVE_NONE:

		if (uuid_mesh != 0) {
			LoadMesh(std::to_string(uuid_mesh));
		}
		else {

			if (!LoadMesh(file.c_str())) {
				AssignMesh(p);
			}
		}

		break;

	case PRIMITIVE_CUBE:
		if (true) {
			par_shapes_mesh* cube = par_shapes_create_cube();
			FillPrimitiveDrawInfo(cube);
			par_shapes_free_mesh(cube);
		}
		break;

	case PRIMITIVE_SPHERE:
		if (true) {
			par_shapes_mesh* sphere = par_shapes_create_subdivided_sphere(3);
			FillPrimitiveDrawInfo(sphere);
			par_shapes_free_mesh(sphere);
		}
		break;

	case PRIMITIVE_PLANE:
		if (true) {
			par_shapes_mesh* plane = par_shapes_create_plane(5, 5);
			FillPrimitiveDrawInfo(plane);
			par_shapes_free_mesh(plane);
		}
		break;
	}
		
	
}