#include "Application.h"
#include "ModuleGui.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/ImGuizmo.h"

#include "ConfigEditor.h"
#include "GameObject.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "SceneSerialization.h"
#include "ModuleScene.h"
#include "ModuleInput.h"

#include "Panel.h"
#include "PanelAbout.h"
#include "PanelConsole.h"
#include "PanelConfig.h"
#include "PanelHierarchy.h"
#include "PanelInspector.h"
#include "PanelAssets.h"
#include "PanelEdit.h"
#include "ModuleRenderer3D.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"
#include "ModuleWindow.h"

#include "NodeGraph.h"

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
	p_assets = new PanelAssets();
	panels.push_back(p_assets);
	p_edit = new PanelEdit();
	panels.push_back(p_edit);

	AddLogToConsole("Initializing ImGui");

	return true;
}
update_status ModuleGui::PreUpdate(float dt)
{
	//Stablish new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	//if (!txt_log_init) {
	//	ImGui::LogToFile();
	//	txt_log_init = true;
	//}

	return UPDATE_CONTINUE;
}

update_status ModuleGui::Update(float dt)
{
	//Shortcuts
	if (App->input->GetKey(SDL_SCANCODE_KP_0) == KEY_DOWN)
		p_edit->ToggleActive();
	if (App->input->GetKey(SDL_SCANCODE_KP_1) == KEY_DOWN)
		p_console->ToggleActive();
	if (App->input->GetKey(SDL_SCANCODE_KP_2) == KEY_DOWN)
		p_hierarchy->ToggleActive();
	if (App->input->GetKey(SDL_SCANCODE_KP_3) == KEY_DOWN)
		p_inspector->ToggleActive();
	if (App->input->GetKey(SDL_SCANCODE_KP_4) == KEY_DOWN)
		p_assets->ToggleActive();
	if (App->input->GetKey(SDL_SCANCODE_KP_5) == KEY_DOWN)
		p_config->ToggleActive();
	
	if (save_scene_clicked)
		SaveSceneMenu();

	if (load_scene_clicked)
		LoadSceneMenu();

	App->node_graph->Draw();

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
	p_assets = nullptr;
	p_edit = nullptr;

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
	static bool save_scene = false;
	static bool load_scene = false;
	bool is_in_editor = true;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("Exit", "ESC", &quit_engine);

			if(App->GetEngineState() != Engine_State_Editor)
				is_in_editor = false;

			if (ImGui::MenuItem("Save scene", "", false, is_in_editor))
				save_scene_clicked = true;
			

			if (ImGui::MenuItem("Load scene", "", false, is_in_editor))
				load_scene_clicked = true;
				

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("GameObject"))
		{
			if (ImGui::MenuItem("Create empty"))
				App->scene->CreateGameObject(App->scene->GetRootGameObject());
			
			if (ImGui::MenuItem("Create camera"))
				App->scene->CreateCamera();

			if (ImGui::MenuItem("Create cube"))
				App->scene->CreatePrimitiveObject(PRIMITIVE_CUBE);
			
			if (ImGui::MenuItem("Create sphere"))
				App->scene->CreatePrimitiveObject(PRIMITIVE_SPHERE);

			if (ImGui::MenuItem("Create plane"))
				App->scene->CreatePrimitiveObject(PRIMITIVE_PLANE);


			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Edit", "KP 0", p_edit->IsActive()))
				p_edit->ToggleActive();

			if(ImGui::MenuItem("Console", "KP 1", p_console->IsActive()))
				p_console->ToggleActive(); 

			if (ImGui::MenuItem("Hierarchy", "KP 2", p_hierarchy->IsActive()))
				p_hierarchy->ToggleActive();

			if (ImGui::MenuItem("Inspector", "KP 3", p_inspector->IsActive()))
				p_inspector->ToggleActive();

			if (ImGui::MenuItem("Assets", "KP 4", p_assets->IsActive()))
				p_assets->ToggleActive();

			if (ImGui::MenuItem("Configuration", "KP 5", p_config->IsActive()))
				p_config->ToggleActive();

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Gui Demo"))
				show_demo_window = !show_demo_window;

			if (ImGui::MenuItem("Documentation"))
				App->RequestBrowser("https://github.com/ValdiviaDev/Stardust-Engine/blob/master/README.md");
			
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

bool ModuleGui::IsMouseHoveringWindow() const
{
	return ImGui::GetIO().WantCaptureMouse;
}


void ModuleGui::SaveSceneMenu() {

	ImGui::OpenPopup("Save scene as...");

	if (ImGui::BeginPopupModal("Save scene as...", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

		ImGui::Text("Scene saved in: Assets/Scenes/");
		
		ImGui::InputText("##SaveSceneName", scene_name, 256);

		if (ImGui::Button("Save", ImVec2(100, 0))) {
			save_scene_clicked = false;
			App->scene->want_to_save = true; 

			ImGui::CloseCurrentPopup();
			
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(100, 0))) {
			save_scene_clicked = false;

			ImGui::CloseCurrentPopup();
			
		}
		ImGui::EndPopup();

	}


}

void ModuleGui::LoadSceneMenu() {

	

	ImGui::OpenPopup("Load scene");

	if (ImGui::BeginPopupModal("Load scene", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

		ImGui::Text("Load scene from: Assets/Scenes/");

		ImGui::InputText("##LoadSceneName", scene_name, 256);

		if (ImGui::Button("Load", ImVec2(100, 0))) {
			load_scene_clicked = false;
			App->scene->want_to_load = true;

			ImGui::CloseCurrentPopup();
			
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(100, 0))) {
			load_scene_clicked = false;

			ImGui::CloseCurrentPopup();

		}
		ImGui::EndPopup();

	}
}

PanelAssets* ModuleGui::GetPanelAssets() const
{
	return p_assets;
}
