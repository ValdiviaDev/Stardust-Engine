#include <time.h>

#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "Parson/parson.h"

#include "MathGeoLib/include/MathGeoLib.h"
#include <gl/GL.h>

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
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



	JSON_Value* root_value = json_value_init_object();
	JSON_Object* root_object = json_value_get_object(root_value);
	root_value = json_parse_file("testing.json");

	if (json_value_get_type(root_value) != JSONArray) {
		LOG("JSON ERROR");
		return;
	}

	
	
	//json_object_set_string(root_object, "name", "John Smith");

	json_value_free(root_value);


	
	//JSON_Value* root_value;
	//JSON_Array* commits;
	//JSON_Object* commit;
	//size_t i;

	//char curl_command[512];
	//char cleanup_command[256];
	//char output_filename[] = "commits.json";

	///* it ain't pretty, but it's not a libcurl tutorial */
	//sprintf(curl_command,
	//	"curl -s \"https://api.github.com/repos/%s/%s/commits\" > %s",
	//	username, repo, output_filename);
	//sprintf(cleanup_command, "rm -f %s", output_filename);
	//system(curl_command);

	///* parsing json and validating output */
	//root_value = json_parse_file(output_filename);
	//if (json_value_get_type(root_value) != JSONArray) {
	//	system(cleanup_command);
	//	return;
	//}

	///* getting array from root value and printing commit info */
	//commits = json_value_get_array(root_value);
	//LOG("%-10.10s %-10.10s %s\n", "Date", "SHA", "Author");
	//for (i = 0; i < json_array_get_count(commits); i++) {
	//	commit = json_array_get_object(commits, i);
	//	LOG("%.10s %.10s %s\n",
	//		json_object_dotget_string(commit, "commit.author.date"),
	//		json_object_get_string(commit, "sha"),
	//		json_object_dotget_string(commit, "commit.author.name"));
	//}

	///* cleanup code */
	//json_value_free(root_value);
	//system(cleanup_command);

	
	

	/*
	
	JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;
    json_object_set_string(root_object, "name", "John Smith");
    json_object_set_number(root_object, "age", 25);
    json_object_dotset_string(root_object, "address.city", "Cupertino");
    json_object_dotset_value(root_object, "contact.emails", json_parse_string("[\"email@example.com\",\"email2@example.com\"]"));
    serialized_string = json_serialize_to_string_pretty(root_value);
    puts(serialized_string);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
	
	*/
}
