#ifndef __ModuleScene_H__
#define __ModuleScene_H__

#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"

#include "Par/par_shapes.h"

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Draw();
	void DrawCubeDirectMode();
	void DrawCubeVertexArray();
	void DrawCubeIndices();

	float GetRandomFloat();
	int GetRandomInt(int a, int b);

	void TryMathGeoLibInters();


	void BufferSimpleGemoetry();


private:
	//RNG test
	LCG lcg;

	//Intersections test
	math::Sphere sp1;
	math::Sphere sp2;

	//Sphere to print
	par_shapes_mesh* sphere;

	uint vert_only_id;
	uint vert_id;
	uint ind_id;
	uint sph_v_id;
	uint sph_i_id;
	uint vao;

	uint num_only_vertex = 1;
	uint num_index = 1;
	

};

#endif