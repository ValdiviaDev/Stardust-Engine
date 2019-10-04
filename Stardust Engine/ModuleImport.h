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
};

class ModuleImport : public Module
{
public: 

	ModuleImport(Application* app, bool start_enabled = true);
	~ModuleImport();

	bool Init(ConfigEditor* config);
	/*bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);*/
	bool CleanUp();
	geo_info LoadMeshVerticesAndIndices();

private:

	


};





#endif