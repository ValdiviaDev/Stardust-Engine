#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;
class ConfigEditor;

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init(ConfigEditor* config);
	bool CleanUp();

	void SetTitle(const char* title);


	void ChangeWindowBrightness(float brightness);
	void ChangeWindowWidth(int width);
	void ChangeWindowHeight(int height);

	int GetScreenRefreshRate();
	void GetWinSize(int& x, int& y) const;

	void SetFullscreen(bool fullscreen);
	void SetResizable(bool resizable);
	void SetBorderless(bool borderless);
	void SetFullDesktop(bool full_desktop);

	void Load(ConfigEditor* config);
	void Save(ConfigEditor* config) const;

	void SetDefaultConfig();


public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;

	//Config
	bool fullscreen = false;
	bool resizable = false;
	bool borderless = false;
	bool full_desktop = false;
	float brightness = 1.0f;
	int win_width = SCREEN_WIDTH;
	int win_height = SCREEN_HEIGHT;

private:
	


};

#endif // __ModuleWindow_H__