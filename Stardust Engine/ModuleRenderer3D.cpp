#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ConfigEditor.h"

#include "Glew/include/glew.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Glew/lib/glew32.lib")

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, "Renderer", start_enabled)
{
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init(ConfigEditor* config)
{
	LOG("Creating 3D Renderer context");
	bool ret = true;
	
	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if (context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	//Init Glew
	GLenum err = glewInit(); // … check for errors 
	if (err != GLEW_OK)
	{
		LOG("Glew library could not init %s\n", glewGetErrorString(err));
		ret = false;
	}

	
	if(ret == true)
	{
		

		//Use Vsync
		if (vsync) {
			SDL_GL_SetSwapInterval(1);
		}
		else
			SDL_GL_SetSwapInterval(0);

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		
		lights[0].Active(true);
		
		Load(config);

		SaveGPUInfo();
	}

	// Projection matrix for
	OnResize(App->window->win_width, App->window->win_height);

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	App->scene->Draw();

	//Debug TODO

	App->gui->Draw();

	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModuleRenderer3D::SaveGPUInfo()
{
	gpu_info.GPU_renderer = (const char*)glGetString(GL_RENDERER);
	gpu_info.GPU_vendor = (const char*)glGetString(GL_VENDOR);

	//LOG("Vendor: %s", glGetString(GL_VENDOR));
	//LOG("Renderer: %s", glGetString(GL_RENDERER));
	//LOG("OpenGL version supported %s", glGetString(GL_VERSION));
	//LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

GPU_Info ModuleRenderer3D::GetGPUInfo()
{
	return gpu_info;
}


void ModuleRenderer3D::Load(ConfigEditor* config) {

	gl_caps.alpha_test = config->ReadBool("AlphaTest", true);
	SetAlphaTest();
	gl_caps.depth_test = config->ReadBool("DepthTest", true);
	SetDepthTest();
	gl_caps.cull_face = config->ReadBool("CullFace", true);
	SetCullFace();
	gl_caps.lighting = config->ReadBool("Lighting", true);
	SetLighting();
	gl_caps.color_material = config->ReadBool("ColorMaterial", true);
	SetColorMaterial();
	gl_caps.texture_2D = config->ReadBool("Texture2D", true);
	SetTexture2D();
	gl_caps.line_smooth = config->ReadBool("LineSmooth", true);
	SetLineSmooth();
	gl_caps.wireframe = config->ReadBool("Wireframe", true);
	SetWireframe();
}


void ModuleRenderer3D::Save(ConfigEditor* config) const {

	config->WriteBool("AlphaTest", gl_caps.alpha_test);
	config->WriteBool("DepthTest", gl_caps.depth_test);
	config->WriteBool("CullFace", gl_caps.cull_face);
	config->WriteBool("Lighting", gl_caps.lighting);
	config->WriteBool("ColorMaterial", gl_caps.color_material);
	config->WriteBool("Texture2D", gl_caps.texture_2D);
	config->WriteBool("LineSmooth", gl_caps.line_smooth);
	config->WriteBool("Wireframe", gl_caps.wireframe);
}


void ModuleRenderer3D::SetAlphaTest() {
	if (gl_caps.alpha_test)
		glEnable(GL_ALPHA_TEST);
	else
		glDisable(GL_ALPHA_TEST);
}

void ModuleRenderer3D::SetDepthTest() {
	if (gl_caps.depth_test)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

void ModuleRenderer3D::SetCullFace() {
	if (gl_caps.cull_face)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}

void ModuleRenderer3D::SetLighting() {
	if (gl_caps.lighting)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);
}

void ModuleRenderer3D::SetColorMaterial() {
	if (gl_caps.color_material)
		glEnable(GL_COLOR_MATERIAL);
	else
		glDisable(GL_COLOR_MATERIAL);
}

void ModuleRenderer3D::SetTexture2D() {
	if (gl_caps.texture_2D)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);
}

void ModuleRenderer3D::SetLineSmooth() {
	if (gl_caps.line_smooth)
		glEnable(GL_LINE_SMOOTH);
	else
		glDisable(GL_LINE_SMOOTH);
}


void ModuleRenderer3D::SetWireframe() {
	if(gl_caps.wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void ModuleRenderer3D::ChangeVsync(bool active)
{
	vsync = active;
	if (vsync) {
		SDL_GL_SetSwapInterval(1);
	}
	else
		SDL_GL_SetSwapInterval(0);
}


void ModuleRenderer3D::SetDefaultConfig() {

	gl_caps.alpha_test = true;
	gl_caps.depth_test = true;
	gl_caps.cull_face = true;
	gl_caps.lighting = true;
	gl_caps.color_material = true;
	gl_caps.texture_2D = true;
	gl_caps.line_smooth = true;
	gl_caps.wireframe = false;

}