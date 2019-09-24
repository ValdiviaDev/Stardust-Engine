#include "Application.h"
#include "ModuleGui.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

ModuleGui::ModuleGui(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleGui::~ModuleGui()
{
}

bool ModuleGui::Init()
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
	about = new PanelAbout();
	panels.push_back(about);

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
		show_app_console = !show_app_console;
	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
		show_app_config = !show_app_config;

	if(show_app_console)
		HandleConsoleWindow();

	if (show_app_config)
		HandleConfigWindow();

	//Draw panel or not
	list<Panel*>::const_iterator panel = panels.begin();
	while (panel != panels.end())
	{
		if ((*panel)->IsActive())
			(*panel)->Draw();
		panel++;
	}

	update_status status = HandleMainMenuBar();

	return status;
}

update_status ModuleGui::PostUpdate(float dt)
{
	//Render
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
	about = nullptr;

	return true;
}

update_status ModuleGui::HandleMainMenuBar()
{
	static bool show_demo_window = false;
	static bool quit_engine = false;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("Quit", "ESC", &quit_engine);

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			ImGui::MenuItem("Console", "1", &show_app_console);
			ImGui::MenuItem("Configuration", "4", &show_app_config);


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
				about->ToggleActive();

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	//Show demo window if we click on the menu
	if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

	if (quit_engine) return UPDATE_STOP;
	
	return UPDATE_CONTINUE;
}

void ModuleGui::HandleConsoleWindow()
{
	//Set window position and size
	ImGui::SetNextWindowPos({ 50,70 }, ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Once);

	//Window
	ImGui::Begin("Console", &show_app_console, ImGuiWindowFlags_None);

	ImGui::End();
}

void ModuleGui::HandleConfigWindow()
{
	//Set window position and size
	ImGui::SetNextWindowPos({ 700,70 }, ImGuiWindowFlags_MenuBar);
	ImGui::SetNextWindowSize(ImVec2(400, 900), ImGuiCond_Once);

	//Window
	ImGui::Begin("Configuration", &show_app_config, ImGuiWindowFlags_None);
	if (ImGui::BeginMenu("Options"))
	{

		ImGui::EndMenu();
	}
	if (ImGui::CollapsingHeader("Aplication"))
	{
		//if(ImGui::Checkbox("Active", true)){}
	}

	if (ImGui::CollapsingHeader("Window"))
	{
		//if(ImGui::Checkbox("Active", true)){}
	}


	ImGui::End();
}
