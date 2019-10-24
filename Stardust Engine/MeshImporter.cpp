#include "MeshImporter.h"
#include "ComponentMesh.h"
#include "ModuleFileSystem.h"

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

bool MeshImporter::Import(const char* file, const char* path, std::string& output_file)
{
	bool ret = false;

	geo_info mesh;

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

	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		aiMesh* new_mesh = scene->mMeshes[0];

		//Material
		aiMaterial* material = scene->mMaterials[new_mesh->mMaterialIndex];
		if (material != nullptr) {
			uint numTextures = material->GetTextureCount(aiTextureType_DIFFUSE);
			aiString mat_path;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &mat_path);

			//go->CreateComponent(Comp_Material);
		}
		// copy vertices
		mesh.num_vertex = new_mesh->mNumVertices;
		mesh.vertex = new float[mesh.num_vertex * 3];
		memcpy(mesh.vertex, new_mesh->mVertices, sizeof(float) * mesh.num_vertex * 3);
		LOG("New mesh with %d vertices", mesh.num_vertex);

		if (mesh.vertex)
			App->gui->AddLogToConsole("Mesh vertices loaded correctly");
		else
			App->gui->AddLogToConsole("ERROR: Mesh vertices not loaded correctly");

		//copy normals
		if (new_mesh->HasNormals()) {
			mesh.num_normal = new_mesh->mNumVertices;
			mesh.normal = new float[mesh.num_normal * 3];
			memcpy(mesh.normal, new_mesh->mNormals, sizeof(float) * mesh.num_normal * 3);
			LOG("New mesh with %d normals", mesh.num_normal);

			if (mesh.normal)
				App->gui->AddLogToConsole("Normals loaded correctly");
			else
				App->gui->AddLogToConsole("ERROR: Normals not loaded correctly");
		}

		//copy uvs
		if (new_mesh->HasTextureCoords(0)) {
			mesh.num_uv = new_mesh->mNumVertices;
			mesh.uv = new float[mesh.num_uv * 2];
			for (uint i = 0; i < new_mesh->mNumVertices; ++i) {
				memcpy(&mesh.uv[i * 2], &new_mesh->mTextureCoords[0][i].x, sizeof(float));
				memcpy(&mesh.uv[(i * 2) + 1], &new_mesh->mTextureCoords[0][i].y, sizeof(float));

			}

			if (mesh.uv)
				App->gui->AddLogToConsole("Texture Coordinates loaded correctly");
			else
				App->gui->AddLogToConsole("ERROR: Texture Coordinates not loaded correctly");
		}


		// copy faces
		if (new_mesh->HasFaces())
		{
			mesh.num_index = new_mesh->mNumFaces * 3;
			mesh.index = new uint[mesh.num_index]; // assume each face is a triangle
			for (uint i = 0; i < new_mesh->mNumFaces; ++i)
			{
				if (new_mesh->mFaces[i].mNumIndices != 3) {
					LOG("WARNING, geometry face with != 3 indices!");
					mesh.has_no_triangle = true;

					if (new_mesh->mFaces[i].mNumIndices < 3)
						App->gui->AddLogToConsole("WARNING, geometry face with < 3 indices!");

					if (new_mesh->mFaces[i].mNumIndices > 3)
						App->gui->AddLogToConsole("WARNING, geometry face with > 3 indices!");
				}
				else
					memcpy(&mesh.index[i * 3], new_mesh->mFaces[i].mIndices, 3 * sizeof(uint));
			}

			if (mesh.index)
				App->gui->AddLogToConsole("Mesh indexes loaded correctly");
			else
				App->gui->AddLogToConsole("ERROR: Mesh indexes not loaded correctly");
		}

		//if (!mesh.has_no_triangle) //TODO will be deprecated soon
		//	SaveDebugData(mesh);

		//Will Probably have to change TODO
		//if (scene->mNumMeshes != 1 && num_mesh < (scene->mNumMeshes - 1)) {
		//	num_mesh++;
		//	GameObject* childGO = App->scene->CreateGameObject(go);
		//	childGO->CreateComponent(Comp_Mesh, path, num_mesh);
		//}

	}

	aiReleaseImport(scene);

	//Save
	uint ranges[4] = { mesh.num_index, mesh.num_vertex, mesh.num_uv, mesh.num_normal };

	uint size = sizeof(ranges) + sizeof(uint) * mesh.num_index + sizeof(float) * mesh.num_vertex * 3 
				+ sizeof(float) * mesh.num_uv * 2 + sizeof(float) * mesh.num_normal * 3;
	
	char* data = new char[size]; // Allocate
	char* cursor = data;
	
	uint bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);
	
	// Store indices
	cursor += bytes; 
	bytes = sizeof(uint) * mesh.num_index;
	memcpy(cursor, mesh.index, bytes);

	// Store vertex
	cursor += bytes;
	bytes = sizeof(uint) * mesh.num_vertex * 3;
	memcpy(cursor, mesh.vertex, bytes);

	// Store UVs
	cursor += bytes;
	bytes = sizeof(uint) * mesh.num_uv * 2;
	memcpy(cursor, mesh.uv, bytes);

	// Store Normals
	cursor += bytes;
	bytes = sizeof(uint) * mesh.num_normal * 3;
	memcpy(cursor, mesh.normal, bytes);

	ret = App->fs->SaveUnique(output_file, data, size, "Library/Meshes/", "own_file_baker", "stdtmesh");

	RELEASE_ARRAY(data);
	RELEASE_ARRAY(mesh.vertex);
	RELEASE_ARRAY(mesh.index);
	RELEASE_ARRAY(mesh.uv);
	RELEASE_ARRAY(mesh.normal);

	return ret;
}

bool MeshImporter::Import(const void* buffer, uint size, std::string& output_file)
{
	return false;
}

bool MeshImporter::Load(const char* exported_file, geo_info& mesh)
{
	bool ret = false;

	char* buffer;
	if (App->fs->Load("Library/Meshes/", exported_file, &buffer) != 0)
		ret = true;

	char* cursor = buffer;

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


	return ret;
}
