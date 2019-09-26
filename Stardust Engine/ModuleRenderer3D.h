#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#define MAX_LIGHTS 8

struct GPU_Info {
	const char* GPU_vendor = nullptr;
	const char* GPU_renderer = nullptr;
	int VRAM_budget = 0;
	int VRAM_usage = 0;
	int VRAM_available = 0;
	int VRAM_reserved = 0;
};

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);

	void SaveGPUInfo();
	GPU_Info GetGPUInfo();

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

private:
	GPU_Info gpu_info;

};