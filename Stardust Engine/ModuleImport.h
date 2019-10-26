#ifndef __ModuleImport__H__
#define __ModuleImport__H__


#include "Module.h"
#include "Par/par_shapes.h"

struct aiScene;
struct aiNode;

struct geo_info {

	//Vertex info
	uint id_vertex = 0;
	uint num_vertex = 0;
	float* vertex = nullptr;

	//Index info
	uint id_index = 0;
	uint num_index = 0;
	uint* index = nullptr;

	//Normals info
	uint id_normal = 0;
	uint num_normal = 0;
	float* normal = nullptr;

	//UVs info
	uint id_uv = 0;
	uint num_uv = 0;
	float* uv = nullptr;

	//Colour vertex info
	uint id_color = 0;
	uint num_color = 0;
	float* color = nullptr;
	
	bool has_no_triangle = false;

	geo_info() {}
};

struct geo_debug {

	vector<float3> vert_point;
	vector<float3> vert_normal;
	
	vector<float3> tri_center;
	vector<float3> tri_normal;


};

class ModuleImport : public Module
{
public: 

	ModuleImport(Application* app, bool start_enabled = true);
	~ModuleImport();

	bool Init(ConfigEditor* config);
	bool Start();
	//update_status PreUpdate(float dt);
	//update_status Update(float dt);
	//update_status PostUpdate(float dt);
	bool CleanUp();
	
	void BindBuffers(geo_info &m);
	void BindBuffersPrimitive(geo_info &a);
	
	//Methods that will stay for the GameObject system
	bool ImportMesh(char* path, geo_info& info, GameObject* go, int num_mesh);
	bool LoadMesh(const aiScene* scene, const aiNode* node, GameObject* parent);

	bool ImportTexture(char* path, uint& tex_id, int& w, int& h);
	void SaveDebugData(geo_info &m);

	list<geo_debug> GetDebugInfo();
	list<geo_debug> m_debug;

};

#endif