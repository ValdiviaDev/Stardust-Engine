#ifndef __ModuleRenderer3D__H__
#define __ModuleRenderer3D__H__

#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#define MAX_LIGHTS 8

#define CHECKERS_HEIGHT 128
#define CHECKERS_WIDTH 128

struct GPU_Info {
	const char* GPU_vendor = nullptr;
	const char* GPU_renderer = nullptr;
	int VRAM_budget = 0;
	int VRAM_usage = 0;
	int VRAM_available = 0;
	int VRAM_reserved = 0;
};

struct GL_Caps {
	bool alpha_test = true;
	bool depth_test = true;
	bool cull_face = true;
	bool lighting = true;
	bool color_material = true;
	bool texture_2D = true;
	bool line_smooth = true;
	bool wireframe = false;
};

struct Render_Debug {
	bool draw_vert_normals = false;
	bool draw_face_normals = false;
	bool draw_tex = true;
	bool draw_checkers_tex = false;
};

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init(ConfigEditor* config);
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);

	void SaveGPUInfo();
	GPU_Info GetGPUInfo() const;

	void Load(ConfigEditor* config);
	void Save(ConfigEditor* config) const;

	void SetAlphaTest();
	void SetDepthTest();
	void SetCullFace();
	void SetLighting();
	void SetColorMaterial();
	void SetTexture2D();
	void SetLineSmooth();
	void SetWireframe();
	void SetVsync(bool value);

	void CreateCheckersTex();
	void SetDefaultConfig();
	
	void DrawModel();
	void DrawModelDebug();

	void UpdateGOMatrix(GameObject* go);

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
	bool vsync = true;

	Render_Debug render_deb;
	GL_Caps gl_caps;

private:
	GPU_Info gpu_info;
	uint checkersImgId;

};

#endif