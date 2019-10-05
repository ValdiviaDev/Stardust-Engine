#ifndef __ModuleImport__H__
#define __ModuleImport__H__

#include "Module.h"

struct geo_info {
	uint id_index = 0; // index in VRAM
	uint num_index = 0;
	uint* index = nullptr;
	uint id_vertex = 0; // unique vertex in VRAM
	uint num_vertex = 0;
	float* vertex = nullptr;

	uint num_normal = 0;
	float* normal = nullptr;

	uint id_uv = 0;
	uint num_uv = 0;
	float* uv = nullptr;
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
	update_status PostUpdate(float dt);
	bool CleanUp();
	void BindBuffers(geo_info &m);
	void ImportFile(char* path);

private:

	geo_info m;


};





#endif