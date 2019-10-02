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

	BufferSimpleGemoetry();


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
	DrawCubeDirectMode();
	DrawCubeVertexArray();
	DrawCubeIndices();
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
	//Draw
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vert_only_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);


	glDrawArrays(GL_TRIANGLES, 0, num_only_vertex);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void ModuleScene::DrawCubeIndices()
{
	//Draw
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, vert_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind_id);
	glDrawElements(GL_TRIANGLES, num_index, GL_UNSIGNED_INT, NULL);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);

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
void ModuleScene::BufferSimpleGemoetry()
{
	//Vertex only
	float vertices[] = { //Front
							-2.0f, 1.0f, 1.0f, //v0 
							-4.0f, 1.0f, 1.0f, //v1
							-4.0f, -1.0f, 1.0f, //v2
							-4.0f, -1.0f, 1.0f, //v2
							-2.0f, -1.0f, 1.0f, //v3
							-2.0f, 1.0f, 1.0f, //v0
							//Right
							-2.0f, 1.0f, 1.0f,
							-2.0f, -1.0f, 1.0f,
							-2.0f, -1.0f, -1.0f,
							-2.0f, -1.0f, -1.0f,
							-2.0f, 1.0f, -1.0f,
							-2.0f, 1.0f, 1.0f,
							//Up
							-2.0f, 1.0f, 1.0f,
							-2.0f, 1.0f, -1.0f,
							-4.0f, 1.0f, -1.0f,
							-4.0f, 1.0f, -1.0f,
							-4.0f, 1.0f, 1.0f,
							-2.0f, 1.0f, 1.0f,
							//Back
							-4.0f, -1.0f, -1.0f,
							-4.0f, 1.0f, -1.0f,
							-2.0f, 1.0f, -1.0f,
							-2.0f, 1.0f, -1.0f,
							-2.0f, -1.0f, -1.0f,
							-4.0f, -1.0f, -1.0f,
							//Left
							-4.0f, -1.0f, -1.0f,
							-4.0f, -1.0f, 1.0f,
							-4.0f, 1.0f, 1.0f,
							-4.0f, 1.0f, 1.0f,
							-4.0f, 1.0f, -1.0f,
							-4.0f, -1.0f, -1.0f,
							//Down
							-4.0f, -1.0f, -1.0f,
							-2.0f, -1.0f, -1.0f,
							-2.0f, -1.0f, 1.0f,
							-2.0f, -1.0f, 1.0f,
							-4.0f, -1.0f, 1.0f,
							-4.0f, -1.0f, -1.0f
	};

	num_only_vertex = sizeof(vertices);
	glGenBuffers(1, (GLuint*) &(vert_only_id));
	glBindBuffer(GL_ARRAY_BUFFER, vert_only_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Vertex and index cube
	float vert[] = {
					4.0f, 1.0f, 1.0f, //v0 
					2.0f, 1.0f, 1.0f, //v1
					2.0f, -1.0f, 1.0f, //v2
					4.0f, -1.0f, 1.0f, //v3
					4.0f, -1.0f, -1.0f, //v4
					4.0f, 1.0f, -1.0f, //v5
					2.0f, 1.0f, -1.0f, //v6
					2.0f, -1.0f, -1.0f //v7
	};

	uint indices[] = { //Front
							  0, 1, 2,
							  2, 3, 0,
							  //Right
							  0, 3, 4,
							  4, 5, 0,
							  //Up
							  0, 5, 6,
							  6, 1, 0,
							  //Back
							  7, 6, 5,
							  5, 4, 7,
							  //Left
							  7, 2, 1,
							  1, 6, 7,
							  //Back
							  7, 4, 3,
							  3, 2, 7
	};
	num_index = sizeof(indices);

	//Vertex
	glGenBuffers(1, (GLuint*) &(vert_id));
	glBindBuffer(GL_ARRAY_BUFFER, vert_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Index
	glGenBuffers(1, (GLuint*) &(ind_id));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}




