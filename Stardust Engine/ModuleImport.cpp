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
#pragma comment (lib, "DevIL/libx86/DevIL.lib")
#pragma comment (lib, "DevIL/libx86/ILU.lib")
#pragma comment (lib, "DevIL/libx86/ILUT.lib")
#include "DevIL/include/il.h"
#include "DevIL/include/ilu.h"
#include "DevIL/include/ilut.h"



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
	m_debug.clear();
	//If there's previous data for a mesh, delete it (temporary)
	
	
	uint flags = 0;
	flags |= aiProcessPreset_TargetRealtime_MaxQuality;
	flags |= aiProcess_Triangulate;
	flags |= aiProcess_SortByPType | aiPrimitiveType_LINE | aiPrimitiveType_POINT;


	const aiScene* scene = aiImportFile(path, flags);
	if (scene)
		App->gui->AddLogToConsole("Assimp scene loaded correctly");
	else
		App->gui->AddLogToConsole("ERROR: Assimp scene not loaded correctly");

	if (scene != nullptr && scene->HasMeshes())
	{
	
		geo_info m;

		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int j = 0; j < scene->mNumMeshes; j++) {

			aiMesh* new_mesh = scene->mMeshes[j];

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
					memcpy(&m.uv[i * 2], &new_mesh->mTextureCoords[0][i].x, sizeof(float));
					memcpy(&m.uv[(i * 2) + 1], &new_mesh->mTextureCoords[0][i].y, sizeof(float));

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
						m.has_no_triangle = true;

						if(new_mesh->mFaces[i].mNumIndices < 3)
							App->gui->AddLogToConsole("WARNING, geometry face with < 3 indices!");
						
						if (new_mesh->mFaces[i].mNumIndices > 3)
							App->gui->AddLogToConsole("WARNING, geometry face with > 3 indices!");
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

			if (!m.has_no_triangle) //Check if theres a face that isn't a triangle
				SaveDebugData(m);

			m_list.push_back(m);
		}
	}


	aiReleaseImport(scene);
}

void ModuleImport::LoadImg()
{
	//Init DevIL
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	//Bind DevIL image
	uint image_id = 0;
	ilGenImages(1, &image_id);
	ilBindImage(image_id);

	//Load image
	const char *path = "Assets/Textures/Baker_house.png";
	if (ilLoad(IL_TYPE_UNKNOWN, path)) {
		LOG("LOADING LENNA");

	}
	else {
		LOG("CANNOT LOAD LENNA");
		ILenum Error;
		while ((Error = ilGetError()) != IL_NO_ERROR) {
			LOG("%d: %s", Error, iluErrorString(Error));
		}
	}

	//Bind DevIL to OpenGL texture buffer
	textureID = ilutGLBindTexImage();

	glBindTexture(GL_TEXTURE_2D, 0);
	ilDeleteImages(1, &image_id);
}

void ModuleImport::ImportMesh(char* path, geo_info& mesh) {

		RELEASE_ARRAY(mesh.index);
		RELEASE_ARRAY(mesh.vertex);
		RELEASE_ARRAY(mesh.normal);
		RELEASE_ARRAY(mesh.uv);
		RELEASE_ARRAY(mesh.color);
		
	//If there's previous data for a mesh, delete it (temporary)


	uint flags = 0;
	flags |= aiProcessPreset_TargetRealtime_MaxQuality;
	flags |= aiProcess_Triangulate;
	flags |= aiProcess_SortByPType | aiPrimitiveType_LINE | aiPrimitiveType_POINT;


	const aiScene* scene = aiImportFile(path, flags);
	if (scene)
		App->gui->AddLogToConsole("Assimp scene loaded correctly");
	else
		App->gui->AddLogToConsole("ERROR: Assimp scene not loaded correctly");

	if (scene != nullptr && scene->HasMeshes())
	{

		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int j = 0; j < scene->mNumMeshes; j++) {

			aiMesh* new_mesh = scene->mMeshes[j];

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

			//copy color
			if (new_mesh->HasVertexColors(0)) {
				mesh.num_color = new_mesh->mNumVertices;
				mesh.color = new float[mesh.num_color * 4];
				for (uint i = 0; i < new_mesh->mNumVertices; ++i) {
					memcpy(&mesh.color[i], &new_mesh->mColors[0][i].r, sizeof(float));
					memcpy(&mesh.color[i + 1], &new_mesh->mColors[0][i].g, sizeof(float));
					memcpy(&mesh.color[i + 2], &new_mesh->mColors[0][i].b, sizeof(float));
					memcpy(&mesh.color[i + 3], &new_mesh->mColors[0][i].a, sizeof(float));
				}

				if (mesh.color)
					App->gui->AddLogToConsole("Color vertex loaded correctly");
				else
					App->gui->AddLogToConsole("ERROR: Color vertex not loaded correctly");
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



			if (!mesh.has_no_triangle) //Check if theres a face that isn't a triangle
				SaveDebugData(mesh);
		}
	}


	aiReleaseImport(scene);

}


void ModuleImport::SaveDebugData(geo_info &m)
{
	geo_debug deb;
	//Face normals
	for (int i = 0; i < m.num_index; i++) {
		//Triangle points
		uint index_01 = m.index[i] * 3;
		uint index_02 = m.index[i + 1] * 3;
		uint index_03 = m.index[i + 2] * 3;

		float3 p1 = { m.vertex[index_01], m.vertex[index_01 + 1], m.vertex[index_01 + 2] };
		float3 p2 = { m.vertex[index_02], m.vertex[index_02 + 1], m.vertex[index_02 + 2] };
		float3 p3 = { m.vertex[index_03], m.vertex[index_03 + 1], m.vertex[index_03 + 2] };

		//Calculate face center
		float C1 = (p1.x + p2.x + p3.x) / 3;
		float C2 = (p1.y + p2.y + p3.y) / 3;
		float C3 = (p1.z + p2.z + p3.z) / 3;

		//Calculate Face Normal
		float3 U = { p2 - p1 };
		float3 V = { p3 - p1 };

		float Nx = U.y*V.z - U.z*V.y;
		float Ny = U.z*V.x - U.x*V.z;
		float Nz = U.x*V.y - U.y*V.x;

		deb.tri_center.push_back({ C1, C2, C3 });
		deb.tri_normal.push_back({ Nx, Ny, Nz });

		i += 2;
	}

	m_debug.push_back(deb);
}

list<geo_info> ModuleImport::GetModel()
{
	return m_list;
}

list<geo_debug> ModuleImport::GetDebugInfo()
{
	return m_debug;
}

bool ModuleImport::Start() {

	//Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
	LoadImg();

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