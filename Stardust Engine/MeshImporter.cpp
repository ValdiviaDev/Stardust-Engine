#include "Application.h"
#include "MeshImporter.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ModuleFileSystem.h"
#include "SceneSerialization.h"
#include "ResourceMesh.h"
#include "ModuleResourceManager.h"
#include "MaterialImporter.h"
#include "ModuleGui.h"
#include "ModuleScene.h"

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

bool MeshImporter::ImportScene(const char* file, const char* path, std::string& output_file, std::vector<UID>& mesh_uuids, bool create_meta,  UID parent_uid, bool in_uids)
{
	bool ret = false;

	string output_f;

	uint flags = 0;
	flags |= aiProcessPreset_TargetRealtime_MaxQuality;
	flags |= aiProcess_Triangulate;
	flags |= aiProcess_SortByPType | aiPrimitiveType_LINE | aiPrimitiveType_POINT;


	const aiScene* scene = aiImportFile(path, flags);
	if (scene) {
#ifndef GAME_MODE
		App->gui->AddLogToConsole("Assimp scene loaded correctly");
#endif
	}
	else {
#ifndef GAME_MODE
		App->gui->AddLogToConsole("ERROR: Assimp scene not loaded correctly");
#endif
		return false;
	}

	if (scene != nullptr)
	{
		const aiNode* root = scene->mRootNode;
		GameObject* dummy = App->scene->CreateGameObject(nullptr);

		if (parent_uid != 0)
			dummy->uuid = parent_uid;
		
		std::list<GameObject*> go_list;

		count_import = 0;
		ret = ImportNodeAndSerialize(scene, root, dummy, dummy->transform, (char*)path, &go_list, mesh_uuids, in_uids);

		//Generate .meta
		if(create_meta)
			App->resources->GenerateMetaFile(path, ResourceType::Resource_Mesh, dummy->uuid, mesh_uuids);



		//Make .json scene for the .fbx scene
		char file_name[100];
		strcpy(file_name, std::to_string(dummy->uuid).c_str());
		strcat(file_name, ".json");
		App->scene_serialization->SaveSceneFromMesh(file_name, go_list);
		

		charged_meshes.clear();
		RELEASE(dummy);
		aiReleaseImport(scene);
	}

	return ret;
}

bool MeshImporter::ImportNodeAndSerialize(const aiScene* scene, const aiNode* node, GameObject* parent, ComponentTransform* transform, char* path, std::list<GameObject*>* go_list, std::vector<UID>& mesh_uuids, bool in_uids)
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

		transform->SumPositionGlobal(go_pos);
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

		//If the mesh has only triangles, save it
		if (HasMeshOnlyTriangles(new_mesh)) {
			//Look if generate random uuid or get the uuid we have
			uint u = App->resources->GenerateNewUID();
			if (in_uids && mesh_uuids.size() > count_import)
				u = mesh_uuids[count_import];
			ResourceMesh* mesh = new ResourceMesh(u);
			
			//Used for serialization ------------------------------------
			ComponentMesh* dummy_c_mesh = (ComponentMesh*)go->CreateComponent(Comp_Mesh);
			dummy_c_mesh->SetPath(path);
			//-----------------------------------------------------------

			//Import Material from the mesh
			aiMaterial* material = scene->mMaterials[new_mesh->mMaterialIndex];
			if (material != nullptr)
				ImportMatFromMesh(material, go);

			//Look if there is a repeated mesh. If there isn't one, import the non-repeated mesh
			if (!IsMeshRepeated(new_mesh, dummy_c_mesh, mesh)) {
				// copy vertices
				mesh->num_vertex = new_mesh->mNumVertices;
				mesh->vertex = new float[mesh->num_vertex * 3];
				memcpy(mesh->vertex, new_mesh->mVertices, sizeof(float) * mesh->num_vertex * 3);
				LOG("New mesh with %d vertices", mesh->num_vertex);

#ifndef GAME_MODE
				if (mesh->vertex)
					App->gui->AddLogToConsole("Mesh vertices loaded correctly");
				else
					App->gui->AddLogToConsole("ERROR: Mesh vertices not loaded correctly");
#endif

				//copy normals
				if (new_mesh->HasNormals()) {
					mesh->num_normal = new_mesh->mNumVertices;
					mesh->normal = new float[mesh->num_normal * 3];
					memcpy(mesh->normal, new_mesh->mNormals, sizeof(float) * mesh->num_normal * 3);
					LOG("New mesh with %d normals", mesh->num_normal);

#ifndef GAME_MODE
					if (mesh->normal)
						App->gui->AddLogToConsole("Normals loaded correctly");
					else
						App->gui->AddLogToConsole("ERROR: Normals not loaded correctly");
#endif
				}

				//copy uvs
				if (new_mesh->HasTextureCoords(0)) {
					mesh->num_uv = new_mesh->mNumVertices;
					mesh->uv = new float[mesh->num_uv * 2];
					for (uint i = 0; i < new_mesh->mNumVertices; ++i) {
						memcpy(&mesh->uv[i * 2], &new_mesh->mTextureCoords[0][i].x, sizeof(float));
						memcpy(&mesh->uv[(i * 2) + 1], &new_mesh->mTextureCoords[0][i].y, sizeof(float));

					}

#ifndef GAME_MODE
					if (mesh->uv)
						App->gui->AddLogToConsole("Texture Coordinates loaded correctly");
					else
						App->gui->AddLogToConsole("ERROR: Texture Coordinates not loaded correctly");
#endif
				}


				// copy faces
				if (new_mesh->HasFaces())
				{
					mesh->num_index = new_mesh->mNumFaces * 3;
					mesh->index = new uint[mesh->num_index]; // assume each face is a triangle
					for (uint i = 0; i < new_mesh->mNumFaces; ++i)
						memcpy(&mesh->index[i * 3], new_mesh->mFaces[i].mIndices, 3 * sizeof(uint));

#ifndef GAME_MODE
					if (mesh->index)
						App->gui->AddLogToConsole("Mesh indexes loaded correctly");
					else
						App->gui->AddLogToConsole("ERROR: Mesh indexes not loaded correctly");
#endif
				}



				//Save a mesh in own file format
				count_import++;

				if (SaveMesh(mesh, std::to_string(mesh->GetUID()).c_str())) {
					if (!in_uids)
						mesh_uuids.push_back(mesh->GetUID()); //Get the mesh UUIDS for the resources
				}
#ifndef GAME_MODE
				else
					App->gui->AddLogToConsole("ERROR: Something failed when trying to save a mesh into Library.");
#endif

				//Delete the auxiliar mesh info (Resource Mesh)
				RELEASE_ARRAY(mesh->vertex);
				RELEASE_ARRAY(mesh->index);
				RELEASE_ARRAY(mesh->normal);
				RELEASE_ARRAY(mesh->uv);
			}
			//Delete the auxiliar Resource Mesh
			RELEASE(mesh);
		}
	}

	//Used for fbx scene serialization
	if (go)
		go_list->push_back(go);

	//Recursive iteration to save all nodes
	for (int i = 0; i < node->mNumChildren; ++i)
		ImportNodeAndSerialize(scene, node->mChildren[i], parent, transform, path, go_list, mesh_uuids, in_uids);

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

bool MeshImporter::HasMeshOnlyTriangles(aiMesh* mesh)
{
	for (uint i = 0; i < mesh->mNumFaces; ++i) {
		if (mesh->mFaces[i].mNumIndices != 3) {
			LOG("WARNING, geometry face with != 3 indices!");
#ifndef GAME_MODE
			if (mesh->mFaces[i].mNumIndices < 3)
				App->gui->AddLogToConsole("WARNING, geometry face with < 3 indices!");

			if (mesh->mFaces[i].mNumIndices > 3)
				App->gui->AddLogToConsole("WARNING, geometry face with > 3 indices!");
#endif

			return false;
		}
	}
	return true;
}

bool MeshImporter::IsMeshRepeated(aiMesh* new_mesh, ComponentMesh* c_mesh, ResourceMesh* r_mesh)
{
	bool save_mesh = true;
	std::map<aiMesh*, UID>::iterator it = charged_meshes.find(new_mesh);
	if (it != charged_meshes.end()) {
		c_mesh->uuid_mesh = it->second;

		return true;
	}
	else {
		c_mesh->uuid_mesh = r_mesh->GetUID();
		charged_meshes[new_mesh] = r_mesh->GetUID();

		return false;
	}

}

void MeshImporter::ImportMatFromMesh(aiMaterial* material, GameObject* go)
{
	//Get texture path
	uint numTextures = material->GetTextureCount(aiTextureType_DIFFUSE);
	aiString mat_path;
	material->GetTexture(aiTextureType_DIFFUSE, 0, &mat_path);
	std::string mat_path_name = (string)mat_path.C_Str();
	std::string mat_path_s = mat_path_name;

	//Find the texture by its name in the textures folder
	if (mat_path_s.find("\\") != std::string::npos)
		mat_path_name = mat_path_s.erase(0, mat_path_s.find_last_of("\\") + 1);
	mat_path_s = ASSETS_TEX_FOLDER + mat_path_name;
	App->fs->NormalizePath(mat_path_s);

	//Create the material if the texture is found
	if (App->fs->Exists(mat_path_s.c_str())) {

		std::string file_meta = mat_path_s + ".meta";
		UID mat_uuid = 0;
		if (App->fs->Exists(file_meta.c_str())) {

			JSON_Value* root_value = json_parse_file(file_meta.c_str());
			JSON_Object* object = json_value_get_object(root_value);

			mat_uuid = json_object_get_number(object, "UUID");
		}


		ComponentMaterial* mat = (ComponentMaterial*)go->CreateComponent(Comp_Material);

		mat_uuid = App->resources->ImportFile(mat_path_s.c_str(), ResourceType::Resource_Texture, mat_uuid);


		if (mat_uuid == 0)
			mat->uuid_mat = App->resources->FindByFileInAssets(mat_path_s.c_str());
		else
			mat->uuid_mat = mat_uuid;

		mat->SetPath(App->mat_import->GetTexturePathFromUUID(mat->uuid_mat));
	
	}
}
