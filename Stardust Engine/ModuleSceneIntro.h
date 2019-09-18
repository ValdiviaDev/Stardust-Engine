#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	//Render primitives
	void RenderPrimitives();

	//Functions to create physics primitives
	Cube CreateCube(vec3 size, vec3 pos, Color color, float mass = 0.0f, bool draw = true, bool isPhysicsObject = true, bool rotate = false, float angleToRot = 0.0f, vec3  axisToRot = (0.0f, 1.0f, 0.0f));
	Cylinder CreateCylinder(float radius, float height, vec3 pos, Color color, float mass = 0.0f, bool draw = true, bool isPhysicsObject = true, bool rotate = false, float angleToRot = 0.0f, vec3  axisToRot = (0.0f, 1.0f, 0.0f));
	Sphere CreateSphere(float radius, vec3 pos, Color color, float mass = 0.0f, bool draw = true, bool isPhysicsObject = true, bool rotate = false, float angleToRot = 0.0f, vec3  axisToRot = (0.0f, 1.0f, 0.0f));


private:

	//Objects to render
	Plane grid;
	p2List<Cube> cubesToRender;
	p2List<Cylinder> cylindersToRender;
	p2List<Sphere> spheresToRender;

};
