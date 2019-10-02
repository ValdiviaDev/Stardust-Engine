#include "PanelConfig.h"
#include "imgui/imgui.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"

PanelConfig::PanelConfig()
{
	name = "config";
	active = true;
	pos_x = 800, pos_y = 70, width = 400, height = 700;
}

PanelConfig::~PanelConfig()
{
}

void PanelConfig::Draw()
{
	//Set window position and size
	ImGui::SetNextWindowPos(ImVec2(pos_x, pos_y), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Once);

	//Window
	ImGui::Begin("Configuration", &active, ImGuiWindowFlags_None);
	if (ImGui::BeginMenu("Options"))
	{
		if (ImGui::MenuItem("Set defaults"))
			App->SetAllConfigToDefault();

		if (ImGui::MenuItem("Save"))
			App->SaveConfig();

		if (ImGui::MenuItem("Load"))
			App->LoadConfig();
			

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

	if (ImGui::CollapsingHeader("Renderer"))
		RendererMenu();

	ImGui::End();
}

void PanelConfig::ApplicationMenu()
{
	static int max_fps = App->GetFPSCap();
	static bool cap_fps = App->GetIfFPSCapping();
	bool vsync = App->renderer3D->vsync;

	//Engine name & organization
	static char eng_name[30] = TITLE;
	if (ImGui::InputText("App Name", eng_name, IM_ARRAYSIZE(eng_name)))
		App->window->SetTitle(eng_name);

	static char org_name[30] = ORGANIZATION;
	ImGui::InputText("Organization", org_name, IM_ARRAYSIZE(org_name));

	//Toggle FPS
	if (ImGui::Checkbox("Cap FPS", &cap_fps))
		App->SetIfFPSCapping(cap_fps);
	ImGui::SameLine();
	if (ImGui::Checkbox("Vsync", &vsync))
		App->renderer3D->SetVsync(vsync);


	if (cap_fps) {
		if (ImGui::SliderInt("Max FPS", &max_fps, 0, 120))
			App->SetFPSCap(max_fps);
	}

	ImGui::Text("Limit framerate: ");
	ImGui::SameLine();
	ImGui::TextColored({ 255,255,0,255 }, "%i", max_fps);

	//Histogram fps/ms
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
	if (resizable) {
		if (ImGui::SliderInt("Width", &w_width, 640, 1920))
			App->window->ChangeWindowWidth(w_width);

		if (ImGui::SliderInt("Height", &w_height, 480, 1080))
			App->window->ChangeWindowHeight(w_height);
	}
	//Refresh Rate
	ImGui::Text("Refresh rate:");
	ImGui::SameLine();
	ImGui::TextColored({ 255,255,0,255 }, "%i", App->window->GetScreenRefreshRate());

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
	static SDL_version sdl_version;
	SDL_VERSION(&sdl_version); //Linked version
	ImGui::Text("SDL Version:");
	ImGui::SameLine();
	ImGui::TextColored({ 255,255,0,255 }, "%i.%i.%i", sdl_version.major, sdl_version.minor, sdl_version.patch);

	ImGui::Separator();

	ImGui::Text("CPUs:");
	ImGui::SameLine();
	ImGui::TextColored({ 255,255,0,255 }, "%i (Cache: %ikb)", App->GetHardwareInfo().CPU_core_num,
					   App->GetHardwareInfo().cache_line_size);
	
	ImGui::Text("System RAM:");
	ImGui::SameLine();
	ImGui::TextColored({ 255,255,0,255 }, "%.3fGb", ((float)App->GetHardwareInfo().RAM_mb / 1000));
	
	
	ImGui::Text("Caps:");
	ImGui::SameLine();
	CheckForCaps();

	ImGui::Separator();
	ImGui::Text("GPU:");
	ImGui::SameLine();
	ImGui::TextColored({ 255,255,0,255 }, App->renderer3D->GetGPUInfo().GPU_renderer);
	
	ImGui::Text("Brand:");
	ImGui::SameLine();
	ImGui::TextColored({ 255,255,0,255 }, App->renderer3D->GetGPUInfo().GPU_vendor);

}

void PanelConfig::RendererMenu()
{
	if (ImGui::Checkbox("GL_DEPTH_TEST", &App->renderer3D->gl_caps.depth_test)){
		App->renderer3D->SetDepthTest();
	}
	if (ImGui::Checkbox("GL_CULL_FACE", &App->renderer3D->gl_caps.cull_face)){
		App->renderer3D->SetCullFace();
	}
	if (ImGui::Checkbox("GL_LIGHTING", &App->renderer3D->gl_caps.lighting)){
		App->renderer3D->SetLighting();
	}
	if (ImGui::Checkbox("GL_COLOR_MATERIAL", &App->renderer3D->gl_caps.color_material)){
		App->renderer3D->SetColorMaterial();
	}
	if (ImGui::Checkbox("GL_TEXTURE_2D", &App->renderer3D->gl_caps.texture_2D)){
		App->renderer3D->SetTexture2D();
	}
	if (ImGui::Checkbox("GL_ALPHA_TEST", &App->renderer3D->gl_caps.alpha_test)) {
		App->renderer3D->SetAlphaTest();
	}
	if (ImGui::Checkbox("GL_LINE_SMOOTH", &App->renderer3D->gl_caps.line_smooth)) {
		App->renderer3D->SetLineSmooth();
	} 
	if (ImGui::Checkbox("WIREFRAME MODE", &App->renderer3D->gl_caps.wireframe)) {
		App->renderer3D->SetWireframe();
	}
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
	Hardware_Info h_info = App->GetHardwareInfo();
	if(h_info.using_AVX)
		ImGui::TextColored({ 255,255,0,255 }, "AVX,");
	ImGui::SameLine();
	if (h_info.using_AVX2)
		ImGui::TextColored({ 255,255,0,255 }, "AVX2,");
	ImGui::SameLine();
	if (h_info.using_3DNow)
		ImGui::TextColored({ 255,255,0,255 }, "3DNow,");
	ImGui::SameLine();
	if (h_info.using_AltiVec)
		ImGui::TextColored({ 255,255,0,255 }, "AltiVec,");
	ImGui::SameLine();
	if (h_info.using_MMX)
		ImGui::TextColored({ 255,255,0,255 }, "MMX,");
	ImGui::SameLine();
	if (h_info.using_RDTSC)
		ImGui::TextColored({ 255,255,0,255 }, "RDTSC,");
	ImGui::SameLine();
	if (h_info.using_SSE)
		ImGui::TextColored({ 255,255,0,255 }, "SSE,");
	ImGui::SameLine();
	if (h_info.using_SSE2)
		ImGui::TextColored({ 255,255,0,255 }, "SSE2,");
	if (h_info.using_SSE3)
		ImGui::TextColored({ 255,255,0,255 }, "SSE3,");
	ImGui::SameLine();
	if (h_info.using_SSE41)
		ImGui::TextColored({ 255,255,0,255 }, "SSE41,");
	ImGui::SameLine();
	if (h_info.using_SSE42)
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



