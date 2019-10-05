#include "Application.h"
#include "ModuleImport.h"
#include "ConfigEditor.h"
#include "Globals.h"
#include "Glew/include/glew.h"
#include <gl/GL.h>
#include "ModuleRenderer3D.h"

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

	

	return true;
}

void ModuleImport::ImportFile(char* path) {

	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{

		aiMesh* new_mesh = scene->mMeshes[0];
		// copy vertices
		m.num_vertex = new_mesh->mNumVertices;
		m.vertex = new float[m.num_vertex * 3];
		memcpy(m.vertex, new_mesh->mVertices, sizeof(float) * m.num_vertex * 3);
		LOG("New mesh with %d vertices", m.num_vertex);
		
		//copy normals
		if (new_mesh->HasNormals()) {
			m.num_normal = new_mesh->mNumVertices;
			m.normal = new float[m.num_normal * 3];
			memcpy(m.normal, new_mesh->mNormals, sizeof(float) * m.num_normal * 3);
			LOG("New mesh with %d normals", m.num_normal);
		}

		//copy uvs
		if (new_mesh->HasTextureCoords(0)) {
			m.num_uv = new_mesh->mNumVertices;
			m.uv = new float[m.num_uv * 2];
			for (uint i = 0; i < new_mesh->mNumVertices; ++i)
			{
				memcpy(&m.uv[i], &new_mesh->mTextureCoords[0][i], sizeof(float) * 2);
			}
		}


		// copy faces
		if (new_mesh->HasFaces())
		{
			m.num_index = new_mesh->mNumFaces * 3;
			m.index = new uint[m.num_index]; // assume each face is a triangle
			for (uint i = 0; i < new_mesh->mNumFaces; ++i)
			{
				if (new_mesh->mFaces[i].mNumIndices != 3) {
					LOG("WARNING, geometry face with != 3 indices!");
					App->gui->AddLogToConsole("WARNING, geometry face with != 3 indices!");
				}
				else
					memcpy(&m.index[i * 3], new_mesh->mFaces[i].mIndices, 3 * sizeof(uint));
			}
		}

		BindBuffers(m);
	}


	// Use scene->mNumMeshes to iterate on scene->mMeshes array


	aiReleaseImport(scene);
}

bool ModuleImport::Start() {

	//Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);


		

	return true;
}

update_status ModuleImport::PostUpdate(float dt) {

	if (m.index != nullptr && m.vertex != nullptr) 
		App->renderer3D->DrawModel(m);

	return UPDATE_CONTINUE;
}

bool ModuleImport::CleanUp() {

	// detach log stream
	aiDetachAllLogStreams();

	//CleanUp for the mesh elements TODO
	delete[] m.vertex;
	m.vertex = nullptr;
	delete[] m.index;
	m.index = nullptr;
	delete[] m.normal;
	m.normal = nullptr;
	delete[] m.uv;
	m.uv = nullptr;

	return true;
}

void ModuleImport::BindBuffers(geo_info &m) {

	
	if (m.index!=nullptr && m.vertex != nullptr) {
	//if (m.num_index != 0 && m.num_vertex != 0) {
		//Vertex
		glGenBuffers(1, (GLuint*) &(m.id_vertex));
		glBindBuffer(GL_ARRAY_BUFFER, m.id_vertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * m.num_vertex, m.vertex, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Index
		glGenBuffers(1, (GLuint*) &(m.id_index));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.id_index);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * m.num_index, m.index, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	//Texture coordinates
	glGenBuffers(1, (GLuint*) &(m.id_uv));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.id_uv);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * 2 * m.num_uv, m.uv, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}