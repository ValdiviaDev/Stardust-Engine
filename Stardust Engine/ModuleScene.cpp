#include <time.h>

#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Primitive.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_internal.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleScene::~ModuleScene()
{}

// Load assets
bool ModuleScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(3.0f, 3.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));
	grid = Plane(0, 1, 0, 0);
	grid.axis = true;


	// imgui init
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL2_Init();

	//Style
	ImGui::StyleColorsDark();

	return ret;
}

update_status ModuleScene::PreUpdate(float dt)
{
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	return(UPDATE_CONTINUE);
}

// Load assets
bool ModuleScene::CleanUp()
{
	LOG("Unloading Intro scene");

	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}

// Update
update_status ModuleScene::Update(float dt)
{
	//Render everything on scene
	RenderPrimitives();

	//Demo window
	bool show_demo_window = true;
	ImGui::ShowDemoWindow(&show_demo_window);

	////Render
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	//Title
	char title[80];
	sprintf_s(title, "Stardust Engine %.1f fps", 1/dt);
	App->window->SetTitle(title);


	return UPDATE_CONTINUE;
}

void ModuleScene::RenderPrimitives()
{
	//Render grid
	grid.Render();

	//Render cubes
	for (p2List_item<Cube>* item = cubesToRender.getFirst(); item; item = item->next)
		item->data.Render();

	//Render cylinders
	for (p2List_item<Cylinder>* item = cylindersToRender.getFirst(); item; item = item->next)
		item->data.Render();

	//Render spheres
	for (p2List_item<Sphere>* item = spheresToRender.getFirst(); item; item = item->next)
		item->data.Render();

}


Cube ModuleScene::CreateCube(vec3 size, vec3 pos, Color color, bool draw, bool rotate, float angleToRot, vec3  axisToRot)
{
	Cube cube(size.x, size.y, size.z);
	cube.SetPos(pos.x, pos.y, pos.z);
	cube.color = color;

	if (rotate)
		cube.SetRotation(angleToRot, vec3(axisToRot.x, axisToRot.y, axisToRot.z));

	if (draw)
		cubesToRender.add(cube);

	return cube;
}

Cylinder ModuleScene::CreateCylinder(float radius, float height, vec3 pos, Color color, bool draw, bool rotate, float angleToRot, vec3 axisToRot)
{
	Cylinder cylinder(radius, height);
	cylinder.SetPos(pos.x, pos.y, pos.z);
	cylinder.color = color;

	if(rotate)
		cylinder.SetRotation(angleToRot, vec3(axisToRot.x, axisToRot.y, axisToRot.z));

	if (draw)
		cylindersToRender.add(cylinder);

	return cylinder;
}

Sphere ModuleScene::CreateSphere(float radius, vec3 pos, Color color, bool draw, bool rotate, float angleToRot, vec3 axisToRot)
{
	Sphere sphere(radius);
	sphere.SetPos(pos.x, pos.y, pos.z);
	sphere.color = color;

	if (rotate)
		sphere.SetRotation(angleToRot, vec3(axisToRot.x, axisToRot.y, axisToRot.z));

	if (draw)
		spheresToRender.add(sphere);

	return sphere;
}