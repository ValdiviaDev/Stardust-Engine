#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();

	void SetTitle(const char* title);


	void ChangeWindowBrightness(float brightness);
	void ChangeWindowWidth(int width);
	void ChangeWindowHeight(int height);

	void SetFullscreen(bool fullscreen);
	void SetResizable(bool resizable);
	void SetBorderless(bool borderless);
	void SetFullDesktop(bool full_desktop);

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;

private:
	int win_width = 0;
	int win_height = 0;

};

#endif // __ModuleWindow_H__