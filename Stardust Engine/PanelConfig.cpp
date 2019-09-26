#include "PanelConfig.h"
#include "imgui/imgui.h"
#include "Application.h"
#include "ModuleWindow.h"

PanelConfig::PanelConfig()
{
	name = "config";
	active = true;
}

PanelConfig::~PanelConfig()
{
}

void PanelConfig::Draw()
{
	//Set window position and size
	ImGui::SetNextWindowPos({ 700,70 }, ImGuiWindowFlags_MenuBar);
	ImGui::SetNextWindowSize(ImVec2(400, 900), ImGuiCond_Once);

	//Window
	ImGui::Begin("Configuration", &active, ImGuiWindowFlags_None);
	if (ImGui::BeginMenu("Options"))
	{
		ImGui::MenuItem("Set defaults"); //TODO
		ImGui::MenuItem("Save"); //TODO
		ImGui::MenuItem("Load"); //TODO

		ImGui::EndMenu();
	}
	if (ImGui::CollapsingHeader("Application"))
		ApplicationMenu();

	if (ImGui::CollapsingHeader("Window"))
		WindowMenu();

	if (ImGui::CollapsingHeader("File System"))
		FileSystemMenu();

	if (ImGui::CollapsingHeader("Input"))
		InputMenu();

	if (ImGui::CollapsingHeader("Hardware"))
		HardwareMenu();

	ImGui::End();
}

void PanelConfig::ApplicationMenu()
{
	//Engine name & organization
	static char eng_name[30] = TITLE;
	if (ImGui::InputText("App Name", eng_name, IM_ARRAYSIZE(eng_name)))
		App->window->SetTitle(eng_name);

	static char org_name[30] = ORGANIZATION;
	ImGui::InputText("Organization", org_name, IM_ARRAYSIZE(org_name));

	//Historiograms fps/ms
	FillFPSVector();
	FillMSVector();

	char title[25];
	sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
	ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
	sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
	ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
}

void PanelConfig::WindowMenu()
{
	static float brighntess = 1.0f;
	static int w_width = SCREEN_WIDTH;
	static int w_height = SCREEN_HEIGHT;

	static bool fullscreen = false;
	static bool resizable = false;
	static bool borderless = false;
	static bool full_desktop = false;
	
	//Brigthness
	if (ImGui::SliderFloat("Brightness", &brighntess, 0.0f, 1.0f))
		App->window->ChangeWindowBrightness(brighntess);

	//Resize window
	if (ImGui::SliderInt("Width", &w_width, 640, 1920))
		App->window->ChangeWindowWidth(w_width);

	if (ImGui::SliderInt("Height", &w_height, 480, 1080))
		App->window->ChangeWindowHeight(w_height);
	
	//Checkboxes for different window flags
	if (ImGui::Checkbox("Fullscreen", &fullscreen))
		App->window->SetFullscreen(fullscreen);
	ImGui::SameLine();
	if (ImGui::Checkbox("Resizable", &resizable))
		App->window->SetResizable(resizable);

	if (ImGui::Checkbox("Borderless", &borderless))
		App->window->SetBorderless(borderless);
	ImGui::SameLine();
	if (ImGui::Checkbox("Full Desktop", &full_desktop))
		App->window->SetFullDesktop(full_desktop);
}

void PanelConfig::FileSystemMenu()
{
}

void PanelConfig::InputMenu()
{
}

void PanelConfig::HardwareMenu()
{
	ImGui::Text("CPUs: %i", SDL_GetCPUCount());
}

void PanelConfig::FillFPSVector()
{
	if (fps_log.size() >= perfor_vec_size) {
		for (int i = 0; i < perfor_vec_size - 1; i++)
			fps_log[i] = fps_log[i + 1];

		fps_log[perfor_vec_size - 1] = App->GetFPS();
	}
	else
		fps_log.push_back(App->GetFPS());
		
}

void PanelConfig::FillMSVector()
{
	if (ms_log.size() >= perfor_vec_size) {
		for (int i = 0; i < perfor_vec_size - 1; i++)
			ms_log[i] = ms_log[i + 1];

		ms_log[perfor_vec_size - 1] = App->GetMS();
	}
	else
		ms_log.push_back(App->GetMS());
}
