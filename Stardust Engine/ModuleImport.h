#ifndef __ModuleImport__H__
#define __ModuleImport__H__

#include "Module.h"

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
	void ImportFile(char* path);

	list<geo_info> GetModel();
	void asdas(list <geo_info> a) {};

private:
	list<geo_info> m_list;
	//geo_info m;

};





#endif