#include "PanelConfig.h"
#include "imgui/imgui.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"

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
		if (ImGui::MenuItem("Save"))
			App->SaveConfig();

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
	static int max_fps = 60;

	//Engine name & organization
	static char eng_name[30] = TITLE;
	if (ImGui::InputText("App Name", eng_name, IM_ARRAYSIZE(eng_name)))
		App->window->SetTitle(eng_name);

	static char org_name[30] = ORGANIZATION;
	ImGui::InputText("Organization", org_name, IM_ARRAYSIZE(org_name));

	//Toggle FPS
	if (ImGui::SliderInt("Max FPS", &max_fps, 0, 120)){} //TODO

	ImGui::Text("Limit framerate: ");
	ImGui::SameLine();
	ImGui::TextColored({ 255,255,0,255 }, "0");

	//Historiograms fps/ms
	FillFPSVector();
	FillMSVector();

	char title[25];
	sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
	ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
	sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
	ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));

	//MEMORY STUFF TODO
}

void PanelConfig::WindowMenu()
{
	//TODO icon

	float brightness = App->window->brightness;
	int w_width = App->window->win_width;
	int w_height = App->window->win_height;

	bool fullscreen = App->window->fullscreen;
	bool resizable = App->window->resizable;
	bool borderless = App->window->borderless;
	bool full_desktop = App->window->full_desktop;
	 


	//Brigthness
	if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f))
		App->window->ChangeWindowBrightness(brightness);

	//Resize window
	if (ImGui::SliderInt("Width", &w_width, 640, 1920))
		App->window->ChangeWindowWidth(w_width);

	if (ImGui::SliderInt("Height", &w_height, 480, 1080))
		App->window->ChangeWindowHeight(w_height);
	
	//TODO Refresh Rate

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
	//TODO
}

void PanelConfig::InputMenu()
{
	ImGui::Text("Mouse Position:");
	ImGui::SameLine();
	ImGui::TextColored({ 255,255,0,255 }, "%i, %i",
		App->input->GetMouseX(), App->input->GetMouseY());

	ImGui::Text("Mouse Motion:");
	ImGui::SameLine();
	ImGui::TextColored({ 255,255,0,255 }, "%i, %i",
		App->input->GetMouseXMotion(), App->input->GetMouseYMotion());

	ImGui::Text("Mouse Wheel:");
	ImGui::SameLine();
	ImGui::TextColored({ 255,255,0,255 }, "%i", App->input->GetMouseZ());

	ImGui::Separator();

	//Input Log
	ImGui::BeginChild("Input Log", {370,70});
	ImGui::TextUnformatted(imput_log_buff.begin());
	InputConsole();
	if (imput_log_scroll){
		ImGui::SetScrollHere(1.0f);
		imput_log_scroll = false;
	}
	ImGui::EndChild();
}

void PanelConfig::HardwareMenu()
{
	ImGui::Text("SDL Version:"); //TODO
	ImGui::Separator();
	
	ImGui::Text("CPUs:");
	ImGui::SameLine();
	ImGui::TextColored({ 255,255,0,255 }, "%i (Cache: %ikb)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());
	
	ImGui::Text("System RAM:");
	ImGui::SameLine();
	ImGui::TextColored({ 255,255,0,255 }, "%.3fGb", ((float)SDL_GetSystemRAM() / 1000));
	
	
	ImGui::Text("Caps:");
	ImGui::SameLine();
	CheckForCaps();

	ImGui::Separator();
	//ImGui::Text("GPU:"); //TODO
	//ImGui::Text("Brand:");
	//ImGui::Text("VRAM Budget:");
	//ImGui::Text("VRAM Usage:");
	//ImGui::Text("VRAM Reserved:");
}

void PanelConfig::InputConsole()
{
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN) {
		imput_log_buff.appendf("Left button down");
		imput_log_buff.appendf("\n");
		imput_log_scroll = true;
	}
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP) {
		imput_log_buff.appendf("Left button release");
		imput_log_buff.appendf("\n");
		imput_log_scroll = true;
	}

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN) {
		imput_log_buff.appendf("Right button down");
		imput_log_buff.appendf("\n");
		imput_log_scroll = true;
	}
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_UP) {
		imput_log_buff.appendf("Right button release");
		imput_log_buff.appendf("\n");
		imput_log_scroll = true;
	}

	if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_DOWN) {
		imput_log_buff.appendf("Middle button down");
		imput_log_buff.appendf("\n");
		imput_log_scroll = true;
	}
	if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_UP) {
		imput_log_buff.appendf("Middle button release");
		imput_log_buff.appendf("\n");
		imput_log_scroll = true;
	}
}

void PanelConfig::CheckForCaps()
{
	if(SDL_HasAVX())
		ImGui::TextColored({ 255,255,0,255 }, "AVX,");
	ImGui::SameLine();
	if (SDL_Has3DNow())
		ImGui::TextColored({ 255,255,0,255 }, "3DNow,");
	ImGui::SameLine();
	if (SDL_HasAVX2())
		ImGui::TextColored({ 255,255,0,255 }, "AVX2,");
	ImGui::SameLine();
	if (SDL_HasAltiVec())
		ImGui::TextColored({ 255,255,0,255 }, "AltiVec,");
	ImGui::SameLine();
	if (SDL_HasMMX())
		ImGui::TextColored({ 255,255,0,255 }, "MMX,");
	ImGui::SameLine();
	if (SDL_HasRDTSC())
		ImGui::TextColored({ 255,255,0,255 }, "RDTSC,");
	ImGui::SameLine();
	if (SDL_HasSSE())
		ImGui::TextColored({ 255,255,0,255 }, "SSE,");
	ImGui::SameLine();
	if (SDL_HasSSE2())
		ImGui::TextColored({ 255,255,0,255 }, "SSE2,");
	if (SDL_HasSSE3())
		ImGui::TextColored({ 255,255,0,255 }, "SSE3,");
	ImGui::SameLine();
	if (SDL_HasSSE41())
		ImGui::TextColored({ 255,255,0,255 }, "SSE41,");
	ImGui::SameLine();
	if (SDL_HasSSE42())
		ImGui::TextColored({ 255,255,0,255 }, "SSE42");
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



