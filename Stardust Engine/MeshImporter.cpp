#include "MeshImporter.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ModuleFileSystem.h"
#include "SceneSerialization.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"



MeshImporter::MeshImporter()
{
}



MeshImporter::~MeshImporter()
{
}

bool MeshImporter::ImportScene(const char* file, const char* path, std::string& output_file, bool serialize)
{
	bool ret = false;

	string output_f;

	uint flags = 0;
	flags |= aiProcessPreset_TargetRealtime_MaxQuality;
	flags |= aiProcess_Triangulate;
	flags |= aiProcess_SortByPType | aiPrimitiveType_LINE | aiPrimitiveType_POINT;


	const aiScene* scene = aiImportFile(path, flags);
	if (scene)
		App->gui->AddLogToConsole("Assimp scene loaded correctly");
	else {
		App->gui->AddLogToConsole("ERROR: Assimp scene not loaded correctly");
		return false;
	}

	if (scene != nullptr)
	{
		const aiNode* root = scene->mRootNode;
		GameObject* dummy = App->scene->CreateGameObject(nullptr);

		if (serialize) {

			std::list<GameObject*> go_list;

			ret = ImportNodeAndSerialize(scene, root, dummy, dummy->transform, (char*)path, output_f, &go_list);

			SceneSerialization s;
			char file_name[100];
			strcpy(file_name, root->mName.C_Str());
			strcat(file_name, ".json");
			s.SaveSceneFromMesh(file_name, go_list);
		}
		else {
			ret = ImportNode(scene, root, dummy, dummy->transform, (char*)path, output_f);
		}


		RELEASE(dummy);
		aiReleaseImport(scene);
	}

	return ret;
}

bool MeshImporter::Import(const void* buffer, uint size, std::string& output_file)
{
	return false;
}

bool MeshImporter::ImportNode(const aiScene* scene, const aiNode* node, GameObject* parent, ComponentTransform* transform, char* path, std::string& output_file)
{
	static int invalid_pos = std::string::npos;
	std::string name = node->mName.C_Str();
	GameObject* go = nullptr;
	ComponentMesh* mesh = nullptr;

	//Transform of the GameObjects loaded from assimp
	//Have to sum the transform of every node because of ghost nodes
	if (node != scene->mRootNode) {
		aiVector3D position;
		aiVector3D scale;
		aiQuaternion rotation;
		node->mTransformation.Decompose(scale, rotation, position);

		float3 go_pos = { position.x, position.y, position.z };
		Quat go_rot = { rotation.x, rotation.y, rotation.z, rotation.w };
		float3 go_scale = { scale.x, scale.y, scale.z };

		transform->SumPosition(go_pos);
		transform->SumRotation(go_rot.ToEulerXYZ() * RADTODEG);
		transform->SumScale(go_scale);
	}

	//Create GameObjects if they aren't invalid transform nodes
	if (name.find("$_") == invalid_pos && node != scene->mRootNode) {

		go = App->scene->CreateGameObject(parent);
		go->SetName(name.c_str());

		//If the gameobject is a visible gameobject set the position that
		//the invalid non printable transform nodes have
		go->transform->SetPosition(transform->GetPosition());
		go->transform->SetRotation(transform->GetRotation());
		go->transform->SetScale(transform->GetScale());

		//Reset the translation for the next gameobject that can be visible
		transform->Reset();

		parent = go;
	}

	//Load mesh attributes process with assimp
	if (node->mNumMeshes > 0) {
		mesh = (ComponentMesh*)go->CreateComponent(Comp_Mesh, nullptr);
		mesh->SetPath(path);

		//Only charge the first mesh for each node
		aiMesh* new_mesh = scene->mMeshes[node->mMeshes[0]];

		//Material
		aiMaterial* material = scene->mMaterials[new_mesh->mMaterialIndex];
		if (material != nullptr) {
			uint numTextures = material->GetTextureCount(aiTextureType_DIFFUSE);
			aiString mat_path;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &mat_path);

			//go->CreateComponent(Comp_Material);
		}
		// copy vertices
		mesh->m_info.num_vertex = new_mesh->mNumVertices;
		mesh->m_info.vertex = new float[mesh->m_info.num_vertex * 3];
		memcpy(mesh->m_info.vertex, new_mesh->mVertices, sizeof(float) * mesh->m_info.num_vertex * 3);
		LOG("New mesh with %d vertices", mesh->m_info.num_vertex);

		if (mesh->m_info.vertex)
			App->gui->AddLogToConsole("Mesh vertices loaded correctly");
		else
			App->gui->AddLogToConsole("ERROR: Mesh vertices not loaded correctly");

		//copy normals
		if (new_mesh->HasNormals()) {
			mesh->m_info.num_normal = new_mesh->mNumVertices;
			mesh->m_info.normal = new float[mesh->m_info.num_normal * 3];
			memcpy(mesh->m_info.normal, new_mesh->mNormals, sizeof(float) * mesh->m_info.num_normal * 3);
			LOG("New mesh with %d normals", mesh->m_info.num_normal);

			if (mesh->m_info.normal)
				App->gui->AddLogToConsole("Normals loaded correctly");
			else
				App->gui->AddLogToConsole("ERROR: Normals not loaded correctly");
		}

		//copy uvs
		if (new_mesh->HasTextureCoords(0)) {
			mesh->m_info.num_uv = new_mesh->mNumVertices;
			mesh->m_info.uv = new float[mesh->m_info.num_uv * 2];
			for (uint i = 0; i < new_mesh->mNumVertices; ++i) {
				memcpy(&mesh->m_info.uv[i * 2], &new_mesh->mTextureCoords[0][i].x, sizeof(float));
				memcpy(&mesh->m_info.uv[(i * 2) + 1], &new_mesh->mTextureCoords[0][i].y, sizeof(float));

			}

			if (mesh->m_info.uv)
				App->gui->AddLogToConsole("Texture Coordinates loaded correctly");
			else
				App->gui->AddLogToConsole("ERROR: Texture Coordinates not loaded correctly");
		}


		// copy faces
		if (new_mesh->HasFaces())
		{
			mesh->m_info.num_index = new_mesh->mNumFaces * 3;
			mesh->m_info.index = new uint[mesh->m_info.num_index]; // assume each face is a triangle
			for (uint i = 0; i < new_mesh->mNumFaces; ++i)
			{
				if (new_mesh->mFaces[i].mNumIndices != 3) {
					LOG("WARNING, geometry face with != 3 indices!");
					mesh->m_info.has_no_triangle = true;

					if (new_mesh->mFaces[i].mNumIndices < 3)
						App->gui->AddLogToConsole("WARNING, geometry face with < 3 indices!");

					if (new_mesh->mFaces[i].mNumIndices > 3)
						App->gui->AddLogToConsole("WARNING, geometry face with > 3 indices!");
				}
				else
					memcpy(&mesh->m_info.index[i * 3], new_mesh->mFaces[i].mIndices, 3 * sizeof(uint));
			}

			if (mesh->m_info.index)
				App->gui->AddLogToConsole("Mesh indexes loaded correctly");
			else
				App->gui->AddLogToConsole("ERROR: Mesh indexes not loaded correctly");
		}
	
		//Save a mesh in own file format
		SaveMesh(mesh, name.c_str(), output_file);

		RELEASE_ARRAY(mesh->m_info.vertex);
		RELEASE_ARRAY(mesh->m_info.index);
		RELEASE_ARRAY(mesh->m_info.uv);
		RELEASE_ARRAY(mesh->m_info.normal);
	}

	//Recursive iteration to save all nodes
	for (int i = 0; i < node->mNumChildren; ++i)
		ImportNode(scene, node->mChildren[i], parent, transform, path, output_file);

	return true;
}

bool MeshImporter::SaveMesh(ComponentMesh* mesh, const char* file_name, std::string & output_file)
{
	//Save
	uint ranges[4] = { mesh->m_info.num_index, mesh->m_info.num_vertex, mesh->m_info.num_uv, mesh->m_info.num_normal };

	uint size = sizeof(ranges) + sizeof(uint) * mesh->m_info.num_index + sizeof(float) * mesh->m_info.num_vertex * 3
		+ sizeof(float) * mesh->m_info.num_uv * 2 + sizeof(float) * mesh->m_info.num_normal * 3;

	char* data = new char[size]; // Allocate
	char* cursor = data;

	uint bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);

	// Store indices
	cursor += bytes;
	bytes = sizeof(uint) * mesh->m_info.num_index;
	memcpy(cursor, mesh->m_info.index, bytes);

	// Store vertex
	cursor += bytes;
	bytes = sizeof(uint) * mesh->m_info.num_vertex * 3;
	memcpy(cursor, mesh->m_info.vertex, bytes);

	// Store UVs
	cursor += bytes;
	bytes = sizeof(uint) * mesh->m_info.num_uv * 2;
	memcpy(cursor, mesh->m_info.uv, bytes);

	// Store Normals
	cursor += bytes;
	bytes = sizeof(uint) * mesh->m_info.num_normal * 3;
	memcpy(cursor, mesh->m_info.normal, bytes);

	App->fs->SaveUnique(output_file, data, size, LIBRARY_MESH_FOLDER, file_name, MESH_EXTENSION);

	//Send the string to the gui to print in a menu
	string mesh_name = file_name;
	App->gui->loaded_meshes.push_back(mesh_name);

	RELEASE_ARRAY(data);

	return true;
}

bool MeshImporter::LoadMesh(const char* exported_file, geo_info& mesh)
{
	bool ret = false;

	char* buffer = nullptr;
	if (App->fs->Load(LIBRARY_MESH_FOLDER, exported_file, &buffer) != 0)
		ret = true;

	char* cursor = buffer;

	if (buffer) {

		// amount of indices / vertices / texture_coords / normals
		uint ranges[4];
		uint bytes = sizeof(ranges);
		memcpy(ranges, cursor, bytes);

		mesh.num_index = ranges[0];
		mesh.num_vertex = ranges[1];
		mesh.num_uv = ranges[2];
		mesh.num_normal = ranges[3];

		// Load indices
		cursor += bytes;
		bytes = sizeof(uint) * mesh.num_index;
		mesh.index = new uint[mesh.num_index];
		memcpy(mesh.index, cursor, bytes);

		// Load vertex
		cursor += bytes;
		bytes = sizeof(float) * mesh.num_vertex * 3;
		mesh.vertex = new float[mesh.num_vertex * 3];
		memcpy(mesh.vertex, cursor, bytes);

		// Load UVs
		cursor += bytes;
		bytes = sizeof(float) * mesh.num_uv * 2;
		mesh.uv = new float[mesh.num_uv * 2];
		memcpy(mesh.uv, cursor, bytes);

		// Load Normal
		cursor += bytes;
		bytes = sizeof(float) * mesh.num_normal * 3;
		mesh.normal = new float[mesh.num_normal * 3];
		memcpy(mesh.normal, cursor, bytes);

	}

	RELEASE_ARRAY(buffer);

	return ret;
}


bool MeshImporter::ImportNodeAndSerialize(const aiScene* scene, const aiNode* node, GameObject* parent, ComponentTransform* transform, char* path, std::string& output_file, std::list<GameObject*>* go_list)
{
	static int invalid_pos = std::string::npos;
	std::string name = node->mName.C_Str();
	GameObject* go = nullptr;
	ComponentMesh* mesh = nullptr;

	//Transform of the GameObjects loaded from assimp
	//Have to sum the transform of every node because of ghost nodes
	if (node != scene->mRootNode) {
		aiVector3D position;
		aiVector3D scale;
		aiQuaternion rotation;
		node->mTransformation.Decompose(scale, rotation, position);

		float3 go_pos = { position.x, position.y, position.z };
		Quat go_rot = { rotation.x, rotation.y, rotation.z, rotation.w };
		float3 go_scale = { scale.x, scale.y, scale.z };

		transform->SumPosition(go_pos);
		transform->SumRotation(go_rot.ToEulerXYZ() * RADTODEG);
		transform->SumScale(go_scale);
	}

	//Create GameObjects if they aren't invalid transform nodes
	if (name.find("$_") == invalid_pos && node != scene->mRootNode) {

		go = App->scene->CreateGameObject(parent);
		go->SetName(name.c_str());

		//If the gameobject is a visible gameobject set the position that
		//the invalid non printable transform nodes have
		go->transform->SetPosition(transform->GetPosition());
		go->transform->SetRotation(transform->GetRotation());
		go->transform->SetScale(transform->GetScale());

		//Reset the translation for the next gameobject that can be visible
		transform->Reset();

		parent = go;
	}

	//Load mesh attributes process with assimp
	if (node->mNumMeshes > 0) {
		mesh = (ComponentMesh*)go->CreateComponent(Comp_Mesh, nullptr);
		mesh->SetPath(path);

		//Only charge the first mesh for each node
		aiMesh* new_mesh = scene->mMeshes[node->mMeshes[0]];

		//Material
		aiMaterial* material = scene->mMaterials[new_mesh->mMaterialIndex];
		if (material != nullptr) {
			uint numTextures = material->GetTextureCount(aiTextureType_DIFFUSE);
			aiString mat_path;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &mat_path);

			//go->CreateComponent(Comp_Material);
		}
		// copy vertices
		mesh->m_info.num_vertex = new_mesh->mNumVertices;
		mesh->m_info.vertex = new float[mesh->m_info.num_vertex * 3];
		memcpy(mesh->m_info.vertex, new_mesh->mVertices, sizeof(float) * mesh->m_info.num_vertex * 3);
		LOG("New mesh with %d vertices", mesh->m_info.num_vertex);

		if (mesh->m_info.vertex)
			App->gui->AddLogToConsole("Mesh vertices loaded correctly");
		else
			App->gui->AddLogToConsole("ERROR: Mesh vertices not loaded correctly");

		//copy normals
		if (new_mesh->HasNormals()) {
			mesh->m_info.num_normal = new_mesh->mNumVertices;
			mesh->m_info.normal = new float[mesh->m_info.num_normal * 3];
			memcpy(mesh->m_info.normal, new_mesh->mNormals, sizeof(float) * mesh->m_info.num_normal * 3);
			LOG("New mesh with %d normals", mesh->m_info.num_normal);

			if (mesh->m_info.normal)
				App->gui->AddLogToConsole("Normals loaded correctly");
			else
				App->gui->AddLogToConsole("ERROR: Normals not loaded correctly");
		}

		//copy uvs
		if (new_mesh->HasTextureCoords(0)) {
			mesh->m_info.num_uv = new_mesh->mNumVertices;
			mesh->m_info.uv = new float[mesh->m_info.num_uv * 2];
			for (uint i = 0; i < new_mesh->mNumVertices; ++i) {
				memcpy(&mesh->m_info.uv[i * 2], &new_mesh->mTextureCoords[0][i].x, sizeof(float));
				memcpy(&mesh->m_info.uv[(i * 2) + 1], &new_mesh->mTextureCoords[0][i].y, sizeof(float));

			}

			if (mesh->m_info.uv)
				App->gui->AddLogToConsole("Texture Coordinates loaded correctly");
			else
				App->gui->AddLogToConsole("ERROR: Texture Coordinates not loaded correctly");
		}


		// copy faces
		if (new_mesh->HasFaces())
		{
			mesh->m_info.num_index = new_mesh->mNumFaces * 3;
			mesh->m_info.index = new uint[mesh->m_info.num_index]; // assume each face is a triangle
			for (uint i = 0; i < new_mesh->mNumFaces; ++i)
			{
				if (new_mesh->mFaces[i].mNumIndices != 3) {
					LOG("WARNING, geometry face with != 3 indices!");
					mesh->m_info.has_no_triangle = true;

					if (new_mesh->mFaces[i].mNumIndices < 3)
						App->gui->AddLogToConsole("WARNING, geometry face with < 3 indices!");

					if (new_mesh->mFaces[i].mNumIndices > 3)
						App->gui->AddLogToConsole("WARNING, geometry face with > 3 indices!");
				}
				else
					memcpy(&mesh->m_info.index[i * 3], new_mesh->mFaces[i].mIndices, 3 * sizeof(uint));
			}

			if (mesh->m_info.index)
				App->gui->AddLogToConsole("Mesh indexes loaded correctly");
			else
				App->gui->AddLogToConsole("ERROR: Mesh indexes not loaded correctly");
		}

		//Save a mesh in own file format
		SaveMesh(mesh, name.c_str(), output_file);

		RELEASE_ARRAY(mesh->m_info.vertex);
		RELEASE_ARRAY(mesh->m_info.index);
		RELEASE_ARRAY(mesh->m_info.uv);
		RELEASE_ARRAY(mesh->m_info.normal);
	}

	if (go)
		go_list->push_back(go);


	//Recursive iteration to save all nodes
	for (int i = 0; i < node->mNumChildren; ++i)
		ImportNodeAndSerialize(scene, node->mChildren[i], parent, transform, path, output_file, go_list);

	return true;
}