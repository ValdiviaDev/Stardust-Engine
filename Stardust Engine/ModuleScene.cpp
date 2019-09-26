#include <time.h>

#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "Parson/parson.h"

#include "MathGeoLib/include/MathGeoLib.h"
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
	sp1.pos = { 0.0, 0.0, 0.0 };
	sp1.r = 7.0;
	sp2.pos = { 14.1, 0.0, 0.0 };
	sp2.r = 7.0;

	return true;
}

// Update
update_status ModuleScene::Update(float dt)
{
	//Grid
	PrintGrid();

	//Trying out MatGeoLib intersections
	//TryMathGeoLibInters();
	
	//Example calling random float between 0-1 and int between 2 numbers
	//LOG("%f", GetRandomFloat());

	//LOG("%i", GetRandomInt(0, 10));
	

	//Test Parson
	if (App->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN)
		TestingParson("ValdiviaDev", "Stardust-Engine");


	


	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{

	return true;
}


void ModuleScene::PrintGrid() {

	

	glBegin(GL_LINES);
	for (int i = -25; i <= 25; i++) {

		glVertex3f(-25, 0, i);
		glVertex3f(25, 0, i);

		glVertex3f(i, 0, -25);
		glVertex3f(i, 0, 25);

		
		glColor3f(255, 255, 255);
		
	}
	glEnd();
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


void ModuleScene::TestingParson(const char* username, const char* repo) {

	App->SaveConfig();



	/*
	JSON_Value *root_value = json_parse_file("config.json");
	
	JSON_Object *root_object = json_value_get_object(root_value);
	//root_value = json_parse_file("testing.json"); //parse_string?
	char buf[256] = "rikarudoo";

	if (root_value == NULL) { //JSONArray) {
		LOG("JSON not found, creating new file");
		root_value = json_value_init_object();
		
	}

	json_object_set_string(json_object(root_value), "testing", buf);
	json_serialize_to_file(root_value, "config.json");

	json_value_free(root_value);
	//json_value_free(root_object); //Does JSON_Object have to be freed? How?
	

	//PARSON examples, delete later

	//JSON_Value *schema = json_parse_string("{\"name\":\"\"}");
	//JSON_Value *user_data = json_parse_file("user_data.json");
	//char buf[256] = "rikarudo";
	//const char *name = NULL;
	//if (user_data == NULL || json_validate(schema, user_data) != JSONSuccess) {
	//	/*puts("Enter your name:");
	//	scanf("%s", buf);*/
	//	
	//	user_data = json_value_init_object();
	//	json_object_set_string(json_object(user_data), "name", buf);
	//	json_serialize_to_file(user_data, "user_data.json");
	//}
	//name = json_object_get_string(json_object(user_data), "name");
	//LOG("Hello, %s.", name);
	//json_value_free(schema);
	//json_value_free(user_data);
	
	

	
	//
	//JSON_Value *root_value = json_value_init_object();
 //   JSON_Object *root_object = json_value_get_object(root_value);
	//root_value = json_parse_file("commits.json");
 //   char *serialized_string = NULL;
 //   json_object_set_string(root_object, "name", "John Smith");
 //   json_object_set_number(root_object, "age", 25);
 //   json_object_dotset_string(root_object, "address.city", "Cupertino");
 //   json_object_dotset_value(root_object, "contact.emails", json_parse_string("[\"email@example.com\",\"email2@example.com\"]"));
 //   serialized_string = json_serialize_to_string_pretty(root_value);
 //   puts(serialized_string);
 //   json_free_serialized_string(serialized_string);
 //   json_value_free(root_value);
	
	
	
}
