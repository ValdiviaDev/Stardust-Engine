#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "ModuleFileSystem.h"
#include "SceneSerialization.h"
#include "Resource.h"
#include "ModuleGui.h"
#include "ModuleResourceManager.h"
#include "ModuleRenderer3D.h"
#include "ModuleScene.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput(Application* app, bool start_enabled) : Module(app, "Input", start_enabled)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
}

// Destructor
ModuleInput::~ModuleInput()
{
	delete[] keyboard;
}

// Called before render is available
bool ModuleInput::Init(ConfigEditor* config)
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate(float dt)
{
	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	mouse_z = 0;

	for(int i = 0; i < 5; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if(mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if(mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	bool quit = false;
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
#ifndef GAME_MODE
		ImGui_ImplSDL2_ProcessEvent(&e);
#endif
		switch(e.type)
		{
			case SDL_MOUSEWHEEL:
			mouse_z = e.wheel.y;
			break;

			case SDL_MOUSEMOTION:
			mouse_x = e.motion.x / SCREEN_SIZE;
			mouse_y = e.motion.y / SCREEN_SIZE;

			mouse_x_motion = e.motion.xrel / SCREEN_SIZE;
			mouse_y_motion = e.motion.yrel / SCREEN_SIZE;
			break;

			case SDL_QUIT:
			quit = true;
			break;

#ifndef GAME_MODE
			case SDL_DROPFILE:
			{
				LOG("File Drop!");
				FileType ft = App->fs->DetermineFileType(e.drop.file);
				switch (ft) {
				case File_Mesh:
					App->gui->AddLogToConsole("Charging fbx scene");
					LOG("Scene for object already exists, load .json.");
					App->scene_serialization->LoadSceneFromMesh(e.drop.file);
					break;
				case File_Material: {
					App->gui->AddLogToConsole("Charging texture");
					UID tex_uid = App->resources->GetUIDFromMeta(e.drop.file);
					App->scene->AssignTexToGameObject(tex_uid);
				}
					break;
				case File_Scene:
					App->gui->AddLogToConsole("Charging scene");
					App->scene_serialization->LoadScene(e.drop.file);

				case File_Meta:
				case File_Unknown:
					App->gui->AddLogToConsole("ERROR: Couldn't charge file");
					break;
				}
			}
			break;
#endif

			case SDL_WINDOWEVENT:
			{
				if (e.window.event == SDL_WINDOWEVENT_RESIZED || e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					App->renderer3D->OnResize(e.window.data1, e.window.data2);
#ifndef GAME_MODE
					App->gui->ResizePanels();
#endif
				}
			}

			

		}
	}

	if(quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP)
		return UPDATE_STOP;

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}