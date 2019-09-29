#include <time.h>

#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "Parson/parson.h"
#include "MathGeoLib/include/MathGeoLib.h"


#include "Glew/include/glew.h"
#include <gl/GL.h>



ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, "Scene", start_enabled)
{
}

ModuleScene::~ModuleScene()
{}

// Load assets
bool ModuleScene::Start()
{
	LOG("Loading Intro assets");

	//Camera position
	App->camera->Move(vec3(3.0f, 3.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));


	LCG::LCG();

	//Spheres positions and radiuses
	sp1.pos = { 0.0f, 0.0f, 0.0f };
	sp1.r = 7.0;
	sp2.pos = { 14.1f, 0.0f, 0.0f };
	sp2.r = 7.0;

	return true;
}

// Update
update_status ModuleScene::Update(float dt)
{

	//Trying out MatGeoLib intersections
	//TryMathGeoLibInters();
	
	//Example calling random float between 0-1 and int between 2 numbers
	//LOG("%f", GetRandomFloat());

	//LOG("%i", GetRandomInt(0, 10));
	

	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{

	return true;
}


void ModuleScene::Draw() {

	//Grid
	glBegin(GL_LINES);
	for (int i = -25; i <= 25; i++) {

		glVertex3f(-25.0f, 0.0f, i);
		glVertex3f(25.0f, 0.0f, i);

		glVertex3f(i, 0.0f, -25.0f);
		glVertex3f(i, 0.0f, 25.0f);

		
		glColor3f(255.0f, 255.0f, 255.0f);
		
	}
	glEnd();
	
	//Cube
	DrawCubeVertexArray();
	

}

void ModuleScene::DrawCubeDirectMode()
{
	glBegin(GL_TRIANGLES);
	//Front face
	glColor3f(255.0f, 0.0f, 0.0f);

	glVertex3f(1.0f, 1.0f, 1.0f); //v0
	glVertex3f(-1.0f, 1.0f, 1.0f); //v1
	glVertex3f(-1.0f, -1.0f, 1.0f); //v2

	glVertex3f(-1.0f, -1.0f, 1.0f); //v2
	glVertex3f(1.0f, -1.0f, 1.0f); //v3
	glVertex3f(1.0f, 1.0f, 1.0f); //v0

	//Right face
	glColor3f(0.0f, 255.0f, 0.0f);

	glVertex3f(1.0f, 1.0f, 1.0f); //v0
	glVertex3f(1.0f, -1.0f, 1.0f); //v3
	glVertex3f(1.0f, -1.0f, -1.0f); //v4

	glVertex3f(1.0f, -1.0f, -1.0f); //v4
	glVertex3f(1.0f, 1.0f, -1.0f); //v5
	glVertex3f(1.0f, 1.0f, 1.0f); //v0

	//Up face
	glColor3f(0.0f, 0.0f, 255.0f);

	glVertex3f(1.0f, 1.0f, 1.0f); //v0
	glVertex3f(1.0f, 1.0f, -1.0f); //v5
	glVertex3f(-1.0f, 1.0f, -1.0f); //v6

	glVertex3f(-1.0f, 1.0f, -1.0f); //v6
	glVertex3f(-1.0f, 1.0f, 1.0f); //v1
	glVertex3f(1.0f, 1.0f, 1.0f); //v0

	//Back face
	glColor3f(255.0f, 255.0f, 0.0f);

	glVertex3f(-1.0f, -1.0f, -1.0f); //v7
	glVertex3f(-1.0f, 1.0f, -1.0f); //v6
	glVertex3f(1.0f, 1.0f, -1.0f); //v5

	glVertex3f(1.0f, 1.0f, -1.0f); //v5
	glVertex3f(1.0f, -1.0f, -1.0f); //v4
	glVertex3f(-1.0f, -1.0f, -1.0f); //v7

	//Left face
	glColor3f(255.0f, 0.0f, 255.0f);

	glVertex3f(-1.0f, -1.0f, -1.0f); //v7
	glVertex3f(-1.0f, -1.0f, 1.0f); //v2
	glVertex3f(-1.0f, 1.0f, 1.0f); //v1

	glVertex3f(-1.0f, 1.0f, 1.0f); //v1
	glVertex3f(-1.0f, 1.0f, -1.0f); //v6
	glVertex3f(-1.0f, -1.0f, -1.0f); //v7

	//Down face
	glColor3f(0.0f, 255.0f, 255.0f);

	glVertex3f(-1.0f, -1.0f, -1.0f); //v7
	glVertex3f(1.0f, -1.0f, -1.0f); //v4
	glVertex3f(1.0f, -1.0f, 1.0f); //v3

	glVertex3f(1.0f, -1.0f, 1.0f); //v3
	glVertex3f(-1.0f, -1.0f, 1.0f); //v2
	glVertex3f(-1.0f, -1.0f, -1.0f); //v7

	glEnd();
}

void ModuleScene::DrawCubeVertexArray()
{							
	static float vertices[] = { //Front
								1.0f, 1.0f, 1.0f, //v0 
								-1.0f, 1.0f, 1.0f, //v1
								-1.0f, -1.0f, 1.0f, //v2
								-1.0f, -1.0f, 1.0f, //v2
								1.0f, -1.0f, 1.0f, //v3
								1.0f, 1.0f, 1.0f, //v0
								//Right
								1.0f, 1.0f, 1.0f, 
								1.0f, -1.0f, 1.0f,
								1.0f, -1.0f, -1.0f,
								1.0f, -1.0f, -1.0f,
								1.0f, 1.0f, -1.0f,
								1.0f, 1.0f, 1.0f,
								//Up
								1.0f, 1.0f, 1.0f,
								1.0f, 1.0f, -1.0f,
								-1.0f, 1.0f, -1.0f,
								-1.0f, 1.0f, -1.0f,
								-1.0f, 1.0f, 1.0f,
								1.0f, 1.0f, 1.0f,
								//Back
								-1.0f, -1.0f, -1.0f,
								-1.0f, 1.0f, -1.0f,
								1.0f, 1.0f, -1.0f,
								1.0f, 1.0f, -1.0f,
								1.0f, -1.0f, -1.0f,
								-1.0f, -1.0f, -1.0f,
								//Left
								-1.0f, -1.0f, -1.0f,
								-1.0f, -1.0f, 1.0f,
								-1.0f, 1.0f, 1.0f,
								-1.0f, 1.0f, 1.0f,
								-1.0f, 1.0f, -1.0f,
								-1.0f, -1.0f, -1.0f,
								//Down
								-1.0f, -1.0f, -1.0f,
								1.0f, -1.0f, -1.0f,
								1.0f, -1.0f, 1.0f,
								1.0f, -1.0f, 1.0f,
								-1.0f, -1.0f, 1.0f,
								-1.0f, -1.0f, -1.0f
													};

	uint my_id = 0;
	glGenBuffers(1, (GLuint*) &(my_id));
	//glBindBuffer(GL_ARRAY_BUFFER, my_id);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, my_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// ... draw other buffers

	glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices));
	glDisableClientState(GL_VERTEX_ARRAY);
}

void ModuleScene::DrawCubeIndices()
{
	static float indices[] = { //Front
								1.0f, 1.0f, 1.0f, //v0 
								-1.0f, 1.0f, 1.0f, //v1
								-1.0f, -1.0f, 1.0f, //v2
													};

	uint my_indices = 0;
	glGenBuffers(1, (GLuint*) &(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, NULL);
}

float ModuleScene::GetRandomFloat() {

	return lcg.Float(0, 1);;
}

int ModuleScene::GetRandomInt(int a, int b) {

	return lcg.Int(a, b);
}

void ModuleScene::TryMathGeoLibInters()
{
	//Testing sphere intersection
	if (sp1.Intersects(sp2)) {
		LOG("true");
	}
	else {
		LOG("false");
	}
}	



