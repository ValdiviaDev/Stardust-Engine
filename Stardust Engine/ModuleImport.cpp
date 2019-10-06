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

		
	for (list<geo_info>::iterator m = m_list.begin(), end = m_list.end(); m != end; ++m) {
		
		RELEASE_ARRAY(m->index);
		RELEASE_ARRAY(m->vertex);
		RELEASE_ARRAY(m->normal);
		RELEASE_ARRAY(m->uv);
		RELEASE_ARRAY(m->color);
	}
	m_list.clear();
	//If there's previous data for a mesh, delete it (temporary)
	
	

	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene)
		App->gui->AddLogToConsole("Assimp scene loaded correctly");
	else
		App->gui->AddLogToConsole("ERROR: Assimp scene not loaded correctly");

	if (scene != nullptr && scene->HasMeshes())
	{
	
		geo_info m;

		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; i++) {

			aiMesh* new_mesh = scene->mMeshes[i];
			// copy vertices
			m.num_vertex = new_mesh->mNumVertices;
			m.vertex = new float[m.num_vertex * 3];
			memcpy(m.vertex, new_mesh->mVertices, sizeof(float) * m.num_vertex * 3);
			LOG("New mesh with %d vertices", m.num_vertex);

			if (m.vertex)
				App->gui->AddLogToConsole("Mesh vertices loaded correctly");
			else
				App->gui->AddLogToConsole("ERROR: Mesh vertices not loaded correctly");

			//copy normals
			if (new_mesh->HasNormals()) {
				m.num_normal = new_mesh->mNumVertices;
				m.normal = new float[m.num_normal * 3];
				memcpy(m.normal, new_mesh->mNormals, sizeof(float) * m.num_normal * 3);
				LOG("New mesh with %d normals", m.num_normal);

				if (m.normal)
					App->gui->AddLogToConsole("Normals loaded correctly");
				else
					App->gui->AddLogToConsole("ERROR: Normals not loaded correctly");
			}

			//copy color
			if (new_mesh->HasVertexColors(0)) {
				m.num_color = new_mesh->mNumVertices;
				m.color = new float[m.num_color * 4];
				for (uint i = 0; i < new_mesh->mNumVertices; ++i) {
					memcpy(&m.color[i], &new_mesh->mColors[0][i].r, sizeof(float));
					memcpy(&m.color[i + 1], &new_mesh->mColors[0][i].g, sizeof(float));
					memcpy(&m.color[i + 2], &new_mesh->mColors[0][i].b, sizeof(float));
					memcpy(&m.color[i + 3], &new_mesh->mColors[0][i].a, sizeof(float));
				}

				if (m.color)
					App->gui->AddLogToConsole("Color vertex loaded correctly");
				else
					App->gui->AddLogToConsole("ERROR: Color vertex not loaded correctly");
			}

			//copy uvs
			if (new_mesh->HasTextureCoords(0)) {
				m.num_uv = new_mesh->mNumVertices;
				m.uv = new float[m.num_uv * 2];
				for (uint i = 0; i < new_mesh->mNumVertices; ++i){
					memcpy(&m.uv[i], &new_mesh->mTextureCoords[0][i].x, sizeof(float));
					memcpy(&m.uv[i], &new_mesh->mTextureCoords[0][i].y, sizeof(float));
					}

				if (m.uv)
					App->gui->AddLogToConsole("Texture Coordinates loaded correctly");
				else
					App->gui->AddLogToConsole("ERROR: Texture Coordinates not loaded correctly");
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

				if (m.index)
					App->gui->AddLogToConsole("Mesh indexes loaded correctly");
				else
					App->gui->AddLogToConsole("ERROR: Mesh indexes not loaded correctly");
			}

			BindBuffers(m);

			m_list.push_back(m);
		}
	}


	aiReleaseImport(scene);
}

list<geo_info> ModuleImport::GetModel()
{
	return m_list;
}

bool ModuleImport::Start() {

	//Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return true;
}

bool ModuleImport::CleanUp() {

	// detach log stream
	aiDetachAllLogStreams();

	//CleanUp for the mesh elements TODO
	for (list<geo_info>::iterator m = m_list.begin(), end = m_list.end(); m != end; ++m) {

		RELEASE_ARRAY(m->index);
		RELEASE_ARRAY(m->vertex);
		RELEASE_ARRAY(m->normal);
		RELEASE_ARRAY(m->uv);
		RELEASE_ARRAY(m->color);
	}
	m_list.clear();

	return true;
}

void ModuleImport::BindBuffers(geo_info &m) {

	
	if (m.index != nullptr && m.vertex != nullptr) {
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

		//Texture coordinates
		if (m.uv != nullptr) {
			glGenBuffers(1, (GLuint*) &(m.id_uv));
			glBindBuffer(GL_ARRAY_BUFFER, m.id_uv);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * m.num_uv, m.uv, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		//Color coordinates
		if (m.color != nullptr) {
			glGenBuffers(1, (GLuint*) &(m.id_color));
			glBindBuffer(GL_ARRAY_BUFFER, m.id_color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * m.num_color, m.color, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}
}