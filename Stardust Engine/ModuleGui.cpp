#include "Application.h"
#include "ModuleGui.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "ConfigEditor.h"
#include "GameObject.h"

#include "Panel.h"
#include "PanelAbout.h"
#include "PanelConsole.h"
#include "PanelConfig.h"
#include "PanelHierarchy.h"
#include "PanelInspector.h"

#include "GameObject.h"

ModuleGui::ModuleGui(Application* app, bool start_enabled) : Module(app, "Gui", start_enabled)
{
}

ModuleGui::~ModuleGui()
{
}

bool ModuleGui::Init(ConfigEditor* config)
{
	//imGui init
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	//Window Style
	ImGui::StyleColorsDark();

	//Panels
	p_about = new PanelAbout();
	panels.push_back(p_about);
	p_console = new PanelConsole();
	panels.push_back(p_console);
	p_config = new PanelConfig();
	panels.push_back(p_config);
	p_hierarchy = new PanelHierarchy();
	panels.push_back(p_hierarchy);
	p_inspector = new PanelInspector();
	panels.push_back(p_inspector);
	
	AddLogToConsole("Initializing ImGui");

	return true;
}

update_status ModuleGui::PreUpdate(float dt)
{
	//Stablish new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return UPDATE_CONTINUE;
}

update_status ModuleGui::Update(float dt)
{
	//Shortcuts
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		p_console->ToggleActive();
	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
		p_config->ToggleActive();

	//Debug REMOVE LATER
	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		AddLogToConsole("SAMPLE LOG");

	return decide_if_update;
}

update_status ModuleGui::PostUpdate(float dt)
{

	return UPDATE_CONTINUE;
}

bool ModuleGui::CleanUp()
{
	//Clearing up imGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	//CleanUp panels
	list<Panel*>::const_iterator panel = panels.begin();
	while (panel != panels.end())
	{
		if ((*panel) != nullptr) {
			delete *panel;
			panels.remove(*panel++);
			continue;
		}
		panel++;
	}
	p_about = nullptr;
	p_console = nullptr;
	p_config = nullptr;
	p_hierarchy = nullptr;
	p_inspector = nullptr;

	return true;
}

void ModuleGui::Draw()
{
	//Draw panel or not
	list<Panel*>::const_iterator panel = panels.begin();
	while (panel != panels.end())
	{
		if ((*panel)->IsActive())
			(*panel)->Draw();
		panel++;
	}

	HandleMainMenuBar();


	//Render
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ModuleGui::HandleMainMenuBar()
{
	static bool show_demo_window = false;
	static bool quit_engine = false;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("Exit", "ESC", &quit_engine);

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("GameObject"))
		{
			if (ImGui::MenuItem("Create empty"))
				App->scene->CreateGameObject(App->scene->GetRootGameObject());
			
			if (ImGui::MenuItem("Create cube"))
				App->scene->CreateCubePrimitive();
			
			if (ImGui::MenuItem("Create sphere"))
				App->scene->CreateSpherePrimitive(3); //TODO: not tested

			if (ImGui::MenuItem("Create plane"))
				App->scene->CreatePlanePrimitive(5,5); //TODO: not tested


			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			if(ImGui::MenuItem("Console", "1", p_console->IsActive()))
				p_console->ToggleActive(); 

			if (ImGui::MenuItem("Configuration", "4", p_config->IsActive()))
				p_config->ToggleActive();

			if (ImGui::MenuItem("Hierarchy", "", p_hierarchy->IsActive()))
				p_hierarchy->ToggleActive();

			if (ImGui::MenuItem("Inspector", "", p_inspector->IsActive()))
				p_inspector->ToggleActive();

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Gui Demo"))
				show_demo_window = !show_demo_window;

			if (ImGui::MenuItem("Documentation"))
				App->RequestBrowser("https://github.com/ValdiviaDev/Stardust-Engine/wiki");
			
			if (ImGui::MenuItem("Download latest"))
				App->RequestBrowser("https://github.com/ValdiviaDev/Stardust-Engine/releases");
			
			if (ImGui::MenuItem("Report a bug"))
				App->RequestBrowser("https://github.com/ValdiviaDev/Stardust-Engine/issues");
			
			if (ImGui::MenuItem("About"))
				p_about->ToggleActive();

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	//Show demo window if we click on the menu
	if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

	if (quit_engine) 
		decide_if_update = UPDATE_STOP;
}

void ModuleGui::AddLogToConsole(const char * log)
{
	if(p_console != nullptr)
		p_console->AddLog(log);
}

void ModuleGui::ResizePanels()
{
	//Resize panels
	list<Panel*>::const_iterator panel = panels.begin();
	while (panel != panels.end())
	{
		if ((*panel)->IsActive())
			(*panel)->ResizePanel();
		panel++;
	}
}
