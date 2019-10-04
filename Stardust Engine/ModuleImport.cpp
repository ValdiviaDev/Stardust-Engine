#include "Application.h"
#include "ModuleImport.h"
#include "ConfigEditor.h"
#include "Globals.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")




ModuleImport::ModuleImport(Application* app, bool start_enabled) : Module(app, "Import", start_enabled)
{
}


ModuleImport::~ModuleImport()
{
}


bool ModuleImport::Init(ConfigEditor* config)
{

	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	const aiScene* scene = aiImportFile("Assets/Meshes/warrior.FBX", aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{

		//TODO: Follow here

		aiMesh* new_mesh = scene->mMeshes[0];
		// copy vertices
		geo_info m;

		m.num_vertex = new_mesh->mNumVertices;
		m.vertex = new float[m.num_vertex * 3];
		memcpy(m.vertex, new_mesh->mVertices, sizeof(float) * m.num_vertex * 3);
		LOG("New mesh with %d vertices", m.num_vertex);




		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		aiReleaseImport(scene);
	}
	else
		LOG("Error loading mesh");


	return true;
}



bool ModuleImport::CleanUp() {

	// detach log stream
	aiDetachAllLogStreams();

	return true;
}

geo_info ModuleImport::LoadMeshVerticesAndIndices() {

	geo_info aux;

	return aux;
}