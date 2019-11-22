#include "Application.h"
#include "MeshImporter.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ModuleFileSystem.h"
#include "SceneSerialization.h"
#include "ResourceMesh.h"


#pragma comment (lib, "Assimp/libx86/assimp.lib")
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

bool MeshImporter::ImportScene(const char* file, const char* path, std::string& output_file, std::vector<UID>& mesh_uuids)
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

		std::list<GameObject*> go_list;

		ret = ImportNodeAndSerialize(scene, root, dummy, dummy->transform, (char*)path, &go_list, mesh_uuids);

		//Generate .meta
		App->resources->GenerateMetaFile(path, ResourceType::Resource_Mesh, dummy->uuid, mesh_uuids);



		//Make .json scene for the .fbx scene
		char file_name[100];
		strcpy(file_name, file);
		strcat(file_name, ".json");
		App->scene_serialization->SaveSceneFromMesh(file_name, go_list);
		

		RELEASE(dummy);
		aiReleaseImport(scene);
	}

	return ret;
}

bool MeshImporter::ImportNodeAndSerialize(const aiScene* scene, const aiNode* node, GameObject* parent, ComponentTransform* transform, char* path, std::list<GameObject*>* go_list, std::vector<UID>& mesh_uuids)
{
	static int invalid_pos = std::string::npos;
	std::string name = node->mName.C_Str();
	GameObject* go = nullptr;

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
		//Only charge the first mesh for each node
		aiMesh* new_mesh = scene->mMeshes[node->mMeshes[0]];

		bool has_triangles = true;
		for (uint i = 0; i < new_mesh->mNumFaces; ++i) {
			if (new_mesh->mFaces[i].mNumIndices != 3) {
				LOG("WARNING, geometry face with != 3 indices!");
				has_triangles = false;

				if (new_mesh->mFaces[i].mNumIndices < 3)
					App->gui->AddLogToConsole("WARNING, geometry face with < 3 indices!");

				if (new_mesh->mFaces[i].mNumIndices > 3)
					App->gui->AddLogToConsole("WARNING, geometry face with > 3 indices!");


			}
		}

		if (has_triangles) {
			ResourceMesh* mesh = new ResourceMesh(App->resources->GenerateNewUID()); //TODO push random uuid or the meta uuid???
			
			//Used for serialization ------------------------------------
			ComponentMesh* dummy_c_mesh = (ComponentMesh*)go->CreateComponent(Comp_Mesh);
			dummy_c_mesh->SetPath(path);
			dummy_c_mesh->uuid_mesh = mesh->GetUID();
			//-----------------------------------------------------------

			//Material
			aiMaterial* material = scene->mMaterials[new_mesh->mMaterialIndex];
			if (material != nullptr) {
				//Get texture path
				uint numTextures = material->GetTextureCount(aiTextureType_DIFFUSE);
				aiString mat_path;
				material->GetTexture(aiTextureType_DIFFUSE, 0, &mat_path);
				string mat_path_name = (string)mat_path.C_Str();
				string mat_path_s = mat_path_name;

				//Find the texture by its name in the textures folder
				if (mat_path_s.find("\\") != string::npos)
					mat_path_name = mat_path_s.erase(0, mat_path_s.find_last_of("\\") + 1);
				mat_path_s = ASSETS_TEX_FOLDER + mat_path_name;
				App->fs->NormalizePath(mat_path_s);

				//Create the material if the texture is found
				if (App->fs->Exists(mat_path_s.c_str())) {
					go->CreateComponent(Comp_Material);
					//go->material->AssignTextureLib(mat_path_s.c_str());
					go->material->uuid_mat = App->GenerateUUID();

					string out_material;
					App->resources->ImportFile(mat_path_s.c_str(), Resource_Texture);
					go->material->SetPath(App->mat_import->GetTexturePathFromUUID(go->material->uuid_mat));

					//Create json with uuid and path for Material
					//App->mat_import->Serialize(go->material);
				}
			}
			// copy vertices
			mesh->num_vertex = new_mesh->mNumVertices;
			mesh->vertex = new float[mesh->num_vertex * 3];
			memcpy(mesh->vertex, new_mesh->mVertices, sizeof(float) * mesh->num_vertex * 3);
			LOG("New mesh with %d vertices", mesh->num_vertex);

			if (mesh->vertex)
				App->gui->AddLogToConsole("Mesh vertices loaded correctly");
			else
				App->gui->AddLogToConsole("ERROR: Mesh vertices not loaded correctly");

			//copy normals
			if (new_mesh->HasNormals()) {
				mesh->num_normal = new_mesh->mNumVertices;
				mesh->normal = new float[mesh->num_normal * 3];
				memcpy(mesh->normal, new_mesh->mNormals, sizeof(float) * mesh->num_normal * 3);
				LOG("New mesh with %d normals", mesh->num_normal);

				if (mesh->normal)
					App->gui->AddLogToConsole("Normals loaded correctly");
				else
					App->gui->AddLogToConsole("ERROR: Normals not loaded correctly");
			}

			//copy uvs
			if (new_mesh->HasTextureCoords(0)) {
				mesh->num_uv = new_mesh->mNumVertices;
				mesh->uv = new float[mesh->num_uv * 2];
				for (uint i = 0; i < new_mesh->mNumVertices; ++i) {
					memcpy(&mesh->uv[i * 2], &new_mesh->mTextureCoords[0][i].x, sizeof(float));
					memcpy(&mesh->uv[(i * 2) + 1], &new_mesh->mTextureCoords[0][i].y, sizeof(float));

				}

				if (mesh-> uv)
					App->gui->AddLogToConsole("Texture Coordinates loaded correctly");
				else
					App->gui->AddLogToConsole("ERROR: Texture Coordinates not loaded correctly");
			}


			// copy faces
			if (new_mesh->HasFaces())
			{
				mesh->num_index = new_mesh->mNumFaces * 3;
				mesh->index = new uint[mesh->num_index]; // assume each face is a triangle
				for (uint i = 0; i < new_mesh->mNumFaces; ++i)
					memcpy(&mesh->index[i * 3], new_mesh->mFaces[i].mIndices, 3 * sizeof(uint));


				if (mesh->index)
					App->gui->AddLogToConsole("Mesh indexes loaded correctly");
				else
					App->gui->AddLogToConsole("ERROR: Mesh indexes not loaded correctly");
			}

			

			//Save a mesh in own file format

			if (mesh)
				if (SaveMesh(mesh, std::to_string(mesh->GetUID()).c_str())) {
					mesh_uuids.push_back(mesh->GetUID()); //Get the mesh UUIDS for the resources
					App->gui->loaded_meshes_uuid.push_back(mesh->GetUID());
				}
				else
					SaveMesh(mesh, name.c_str());


			

			//Delete the auxiliar mesh info (Resource Mesh)
			RELEASE_ARRAY(mesh->vertex);
			RELEASE_ARRAY(mesh->index);
			RELEASE_ARRAY(mesh->normal);
			RELEASE_ARRAY(mesh->uv);
			RELEASE(mesh);
		}
	}

	if (go)
		go_list->push_back(go);


	//Recursive iteration to save all nodes
	for (int i = 0; i < node->mNumChildren; ++i)
		ImportNodeAndSerialize(scene, node->mChildren[i], parent, transform, path, go_list, mesh_uuids);

	return true;
}

bool MeshImporter::SaveMesh(ResourceMesh* mesh, const char* file_name)
{
	bool ret = false;

	//Save
	uint ranges[4] = { mesh->num_index, mesh->num_vertex, mesh->num_uv, mesh->num_normal };

	uint size = sizeof(ranges) + sizeof(uint) * mesh->num_index + sizeof(float) * mesh->num_vertex * 3
		+ sizeof(float) * mesh->num_uv * 2 + sizeof(float) * mesh->num_normal * 3;

	char* data = new char[size]; // Allocate
	char* cursor = data;

	uint bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);
	cursor += bytes;

	// Store indices
	bytes = sizeof(uint) * mesh->num_index;
	memcpy(cursor, mesh->index, bytes);
	cursor += bytes;

	// Store vertex
	bytes = sizeof(uint) * mesh->num_vertex * 3;
	memcpy(cursor, mesh->vertex, bytes);
	cursor += bytes;

	// Store UVs
	if (mesh->num_uv > 0) {
		bytes = sizeof(uint) * mesh->num_uv * 2;
		memcpy(cursor, mesh->uv, bytes);
		cursor += bytes;
	}

	// Store Normals
	if (mesh->num_normal > 0) {
		bytes = sizeof(uint) * mesh->num_normal * 3;
		memcpy(cursor, mesh->normal, bytes);
	}

	string file_n_ext;
	if (App->fs->SaveUnique(file_n_ext, data, size, LIBRARY_MESH_FOLDER, file_name, MESH_EXTENSION)) //File name is a uuid
		ret = true;


	RELEASE_ARRAY(data);

	return ret;
}

bool MeshImporter::LoadMesh(const char* exported_file, ResourceMesh* mesh)
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

		mesh->num_index = ranges[0];
		mesh->num_vertex = ranges[1];
		mesh->num_uv = ranges[2];
		mesh->num_normal = ranges[3];
		cursor += bytes;

		// Load indices
		bytes = sizeof(uint) * mesh->num_index;
		mesh->index = new uint[mesh->num_index];
		memcpy(mesh->index, cursor, bytes);
		cursor += bytes;

		// Load vertex
		bytes = sizeof(float) * mesh->num_vertex * 3;
		mesh->vertex = new float[mesh->num_vertex * 3];
		memcpy(mesh->vertex, cursor, bytes);
		cursor += bytes;

		// Load UVs
		if (mesh->num_uv > 0) {
			bytes = sizeof(float) * mesh->num_uv * 2;
			mesh->uv = new float[mesh->num_uv * 2];
			memcpy(mesh->uv, cursor, bytes);
			cursor += bytes;
		}

		// Load Normal
		if (mesh->num_normal > 0) {
			bytes = sizeof(float) * mesh->num_normal * 3;
			mesh->normal = new float[mesh->num_normal * 3];
			memcpy(mesh->normal, cursor, bytes);
		}

	}

	RELEASE_ARRAY(buffer);

	return ret;
}
