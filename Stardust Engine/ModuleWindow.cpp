#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ConfigEditor.h"
#include "ModuleRenderer3D.h"

ModuleWindow::ModuleWindow(Application* app, bool start_enabled) : Module(app, "Window", start_enabled)
{
	window = NULL;
	screen_surface = NULL;
	win_width = SCREEN_WIDTH;
	win_height = SCREEN_HEIGHT;
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init(ConfigEditor* config)
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;


		resizable = config->ReadBool("Resizable", true);
		SetResizable(resizable);
		if(resizable)
			flags |= SDL_WINDOW_RESIZABLE;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

			

		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, win_width, win_height, flags);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
		
		Load(config);
	}

	

	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

void ModuleWindow::ChangeWindowBrightness(float brightness)
{
	this->brightness = brightness;
	SDL_SetWindowBrightness(window, brightness);
}

void ModuleWindow::ChangeWindowWidth(int width)
{
	win_width = width;
	SDL_SetWindowSize(window, width, win_height);
	App->renderer3D->OnResize(width, win_height);
}

void ModuleWindow::ChangeWindowHeight(int height)
{
	win_height = height;
	SDL_SetWindowSize(window, win_width, height);
	App->renderer3D->OnResize(win_width, height);
}

int ModuleWindow::GetScreenRefreshRate()
{
	SDL_DisplayMode Mode;
	int DisplayIndex = SDL_GetWindowDisplayIndex(window);
	
	SDL_GetDesktopDisplayMode(DisplayIndex, &Mode);

	if (Mode.refresh_rate == 0)
	{
		return 0; //If error, return 0
	}
	return Mode.refresh_rate;
}

void ModuleWindow::GetWinSize(int& x, int& y) const
{
	SDL_GetWindowSize(window, &x, &y);
}

void ModuleWindow::SetFullscreen(bool fullscreen)
{
	this->fullscreen = fullscreen;
	if (fullscreen)
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	else
		SDL_SetWindowFullscreen(window, 0);
}

void ModuleWindow::SetResizable(bool resizable)
{
	this->resizable = resizable;
	//SDL_SetWindowResizable() SDL ver 2.0.5
}

void ModuleWindow::SetBorderless(bool borderless)
{
	this->borderless = borderless;
	if(borderless)
		SDL_SetWindowBordered(window, (SDL_bool)false);
	else
		SDL_SetWindowBordered(window, (SDL_bool)true);
}

void ModuleWindow::SetFullDesktop(bool full_desktop)
{
	this->full_desktop = full_desktop;
	if (full_desktop)
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	else
		SDL_SetWindowFullscreen(window, 0);
}


void ModuleWindow::Load(ConfigEditor* config) {

	fullscreen = config->ReadBool("Fullscreen", false);
	SetFullscreen(fullscreen);
	resizable = config->ReadBool("Resizable", true);
	SetResizable(resizable);
	borderless = config->ReadBool("Borderless", false);
	SetBorderless(borderless);
	full_desktop = config->ReadBool("Full Desktop", false);
	SetFullDesktop(full_desktop);

	win_width = config->ReadInt("Width", SCREEN_WIDTH * SCREEN_SIZE);
	ChangeWindowWidth(win_width);
	win_height = config->ReadInt("Height", SCREEN_HEIGHT * SCREEN_SIZE);
	ChangeWindowHeight(win_height);
	brightness = config->ReadFloat("Brightness", 1.0f);
	ChangeWindowBrightness(brightness);
}

void ModuleWindow::Save(ConfigEditor* config) const {

	config->WriteBool("Fullscreen", fullscreen);
	config->WriteBool("Resizable", resizable);
	config->WriteBool("Borderless", borderless);
	config->WriteBool("Full Desktop", full_desktop);

	config->WriteInt("Width", win_width);
	config->WriteInt("Height", win_height);
	config->WriteFloat("Brightness", brightness);
}


void ModuleWindow::SetDefaultConfig() {

	fullscreen = false;
	resizable = false;
	borderless = false;
	full_desktop = false;
	brightness = 1.0f;
	win_width = SCREEN_WIDTH;
	win_height = SCREEN_HEIGHT;

	
}