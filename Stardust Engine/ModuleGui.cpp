#include "Application.h"
#include "ModuleGui.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Glew/include/glew.h"
#pragma comment (lib, "Glew/lib/glew32.lib")


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
	ImGui_ImplOpenGL2_Init();

	//Window Style
	ImGui::StyleColorsDark();

	return true;
}

update_status ModuleGui::PreUpdate(float dt)
{
	//Stablish new frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();


	static bool top_menu_bar = false;
	static bool show_app_main_menu_bar = false;
	static bool show_app_console = false;
	static bool show_app_metrics = false;
	static bool show_app_style_editor = false;
	static bool show_demo_window = false;
	static bool quit_engine = false;

	//Set top bar window position and size
	ImGui::SetNextWindowPos({ 0,0 }, ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH * 2, 10), ImGuiCond_Once);

	//Creating menu bar at the top
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_MenuBar;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;

	//Window
	ImGui::Begin("Top Menu", &top_menu_bar, window_flags);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			ImGui::MenuItem("Something");
			ImGui::MenuItem("Console", NULL, &show_app_console);
			ImGui::MenuItem("Quit", NULL, &quit_engine);

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("Metrics", NULL, &show_app_metrics);
			ImGui::MenuItem("Show Demo Window", NULL, &show_demo_window);


			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();

	}
	ImGui::End();

	//Show demo window if we click on the menu
	if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

	if (quit_engine) return UPDATE_STOP;

	return (UPDATE_CONTINUE);
}

update_status ModuleGui::PostUpdate(float dt)
{
	//Render
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	return UPDATE_CONTINUE;
}

bool ModuleGui::CleanUp()
{
	//Clearing up imGui
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}
