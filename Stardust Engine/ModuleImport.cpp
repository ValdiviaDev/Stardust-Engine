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

#include "GameObject.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "Par/par_shapes.h"

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

bool ModuleImport::ImportMesh(char* path, geo_info& mesh, GameObject* go) {

	uint flags = 0;
	flags |= aiProcessPreset_TargetRealtime_MaxQuality;
	flags |= aiProcess_Triangulate;
	flags |= aiProcess_SortByPType | aiPrimitiveType_LINE | aiPrimitiveType_POINT;


	string mesh_str = "Opening file: " + (string)path;
	App->gui->AddLogToConsole(mesh_str.c_str());

	const aiScene* scene = aiImportFile(path, flags);
	const aiNode* root = scene->mRootNode;

	if (scene)
		App->gui->AddLogToConsole("Assimp scene loaded correctly");
	else {
		App->gui->AddLogToConsole("ERROR: Assimp scene not loaded correctly");
		return false;
	}

	if (scene != nullptr && scene->HasMeshes())
	{
		LoadMesh(scene, root, go, go->transform, path);
	}

	aiReleaseImport(scene);

	return true;
}

bool ModuleImport::LoadMesh(const aiScene* scene, const aiNode* node, GameObject* parent, ComponentTransform* transform, char* path)
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

	if (name.find("$_") == invalid_pos && node != scene->mRootNode) {

		go = App->scene->CreateGameObject(parent);
		mesh = (ComponentMesh*)go->CreateComponent(Comp_Mesh, nullptr);
		mesh->SetPath(path);
		
		go->SetName(name.c_str());

		//If the gameobject is a visible gameobject set the position that
		//the invalid non printable transform nodes have
		go->transform->SetPosition(transform->GetPosition());
		go->transform->SetRotation(transform->GetRotation());
		go->transform->SetScale(transform->GetScale());

		//Reset the translation for the next gameobject that can be visible
		transform->SetPosition({ 0.0f, 0.0f, 0.0f });
		transform->SetRotation({ 0.0f, 0.0f, 0.0f });
		transform->SetScale({ 1.0f, 1.0f, 1.0f });

		parent = go;
	}

	//Saving everything for the mesh info
	if (node->mNumMeshes > 0) {

		aiMesh* new_mesh = scene->mMeshes[node->mMeshes[0]];

		//Material
		aiMaterial* material = scene->mMaterials[new_mesh->mMaterialIndex];
		if (material != nullptr) {
			uint numTextures = material->GetTextureCount(aiTextureType_DIFFUSE);
			aiString mat_path;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &mat_path);

			go->CreateComponent(Comp_Material);
		}
		// copy vertices
		mesh->m_info.num_vertex = new_mesh->mNumVertices;
		mesh->m_info.vertex = new float[mesh->m_info.num_vertex * 3];
		memcpy(mesh->m_info.vertex, new_mesh->mVertices, sizeof(float) * mesh->m_info.num_vertex * 3);
		LOG("New mesh with %d vertices", mesh->m_info.num_vertex);

		if (mesh->m_info.vertex) {
			App->gui->AddLogToConsole("Mesh vertices loaded correctly");
			string str = "Mesh Vertices num: " + std::to_string(mesh->m_info.num_vertex);
			App->gui->AddLogToConsole(str.c_str());
		}
		else
			App->gui->AddLogToConsole("ERROR: Mesh vertices not loaded correctly");

		//copy normals
		if (new_mesh->HasNormals()) {
			mesh->m_info.num_normal = new_mesh->mNumVertices;
			mesh->m_info.normal = new float[mesh->m_info.num_normal * 3];
			memcpy(mesh->m_info.normal, new_mesh->mNormals, sizeof(float) * mesh->m_info.num_normal * 3);
			LOG("New mesh with %d normals", mesh->m_info.num_normal);

			if (mesh->m_info.normal) {
				App->gui->AddLogToConsole("Normals loaded correctly");
				string str = "Mesh Normals num: " + std::to_string(mesh->m_info.num_normal);
				App->gui->AddLogToConsole(str.c_str());
			}
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

			if (mesh->m_info.uv) {
				App->gui->AddLogToConsole("Texture Coordinates loaded correctly");
				string str = "Mesh UVs num: " + std::to_string(mesh->m_info.num_uv);
				App->gui->AddLogToConsole(str.c_str());
			}
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

			if (mesh->m_info.index) {
				App->gui->AddLogToConsole("Mesh indexes loaded correctly");
				string str = "Mesh index num: " + std::to_string(mesh->m_info.num_index);
				App->gui->AddLogToConsole(str.c_str());
			}
			else
				App->gui->AddLogToConsole("ERROR: Mesh indexes not loaded correctly");
		}

		BindBuffers(mesh->m_info);
	}

	for (int i = 0; i < node->mNumChildren; ++i)
		LoadMesh(scene, node->mChildren[i], parent, transform, path);

	return true;
}

bool ModuleImport::ImportTexture(char* path, uint& tex_id, int& w, int& h)
{
	//Bind DevIL image
	uint image_id = 0;
	ilGenImages(1, &image_id);
	ilBindImage(image_id);

	string tex_str = "Loading texture: " + (string)path;
	App->gui->AddLogToConsole(tex_str.c_str());

	//Load image
	if (ilLoad(IL_TYPE_UNKNOWN, path)) {
		LOG("Texture Loaded Correctly");
		App->gui->AddLogToConsole("Texture Loaded Correctly");
	}
	else {
		LOG("Error: Couldn't load texture");
		App->gui->AddLogToConsole("Error: Couldn't load texture");
		ILenum Error;
		while ((Error = ilGetError()) != IL_NO_ERROR) {
			LOG("%d: %s", Error, iluErrorString(Error));
		}
		ilDeleteImages(1, &image_id);
		return false;
	}

	//Get width and height
	w = ilGetInteger(IL_IMAGE_WIDTH);
	h = ilGetInteger(IL_IMAGE_HEIGHT);
	string size_str = "Texture size: " + std::to_string(w) + " x " + std::to_string(h);
	App->gui->AddLogToConsole(size_str.c_str());

	//Bind DevIL to OpenGL texture buffer
	tex_id = ilutGLBindTexImage();
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);
	ilDeleteImages(1, &image_id);

	return true;
}


void ModuleImport::SaveDebugData(geo_info &m) //TODO: Soon to be deprecated
{
	geo_debug deb;

	//Vertex normals
	for (int i = 0; i < m.num_normal * 3; i++) {

		//Normalize the vertex normals
		float3 norm = { m.normal[i], m.normal[i + 1], m.normal[i + 2] };
		float  mod = sqrt(norm.x * norm.x + norm.y * norm.y + norm.z * norm.z);
		norm = (norm / mod) * 0.5;

		deb.vert_point.push_back({ m.vertex[i], m.vertex[i + 1], m.vertex[i + 2] });
		deb.vert_normal.push_back({ m.vertex[i] + norm.x, m.vertex[i + 1] + norm.y, 
								    m.vertex[i + 2] + norm.z });
		
		i += 2;
	}

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

		//Normalize the face normals
		float  mod = sqrt(Nx * Nx + Ny * Ny + Nz * Nz);
		Nx = (Nx / mod) * 0.5;
		Ny = (Ny / mod) * 0.5;
		Nz = (Nz / mod) * 0.5;

		deb.tri_center.push_back({ C1, C2, C3 });
		deb.tri_normal.push_back({ Nx, Ny, Nz });

		i += 2;
	}

	m_debug.push_back(deb);
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
	
	//Init DevIL
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	return true;
}

bool ModuleImport::CleanUp() {

	// detach log stream
	aiDetachAllLogStreams();

	return true;
}

void ModuleImport::BindBuffers(geo_info &m) {
	
	if (m.index != nullptr && m.vertex != nullptr) {
		//Vertex
		glGenBuffers(1, (GLuint*) &(m.id_vertex));
		glBindBuffer(GL_ARRAY_BUFFER, m.id_vertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * m.num_vertex, m.vertex, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Normal
		glGenBuffers(1, (GLuint*) &(m.id_normal));
		glBindBuffer(GL_ARRAY_BUFFER, m.id_normal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * m.num_normal, m.normal, GL_STATIC_DRAW);
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
		//if (m.color != nullptr) {
		//	glGenBuffers(1, (GLuint*) &(m.id_color));
		//	glBindBuffer(GL_ARRAY_BUFFER, m.id_color);
		//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * m.num_color, m.color, GL_STATIC_DRAW);
		//	glBindBuffer(GL_ARRAY_BUFFER, 0);
		//}
	}
}

void ModuleImport::BindBuffersPrimitive(geo_info &a)
{
	//Vertex
	glGenBuffers(1, (GLuint*) &(a.id_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, a.id_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * a.num_vertex, a.vertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Index
	glGenBuffers(1, (GLuint*) &(a.id_index));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, a.id_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(PAR_SHAPES_T) * 3 * a.num_index, a.index, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


