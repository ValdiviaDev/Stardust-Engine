#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ConfigEditor.h"

#include "Glew/include/glew.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

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


	//TEST-----------------------------------------------------------------

	GLubyte checkImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &ImgId);
	glBindTexture(GL_TEXTURE_2D, ImgId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_HEIGHT, CHECKERS_WIDTH,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	glBindTexture(GL_TEXTURE_2D, 0);

	//TEST-----------------------------------------------------------------

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

	if (!App->importer->GetModel().empty()) {
		DrawModel();
		DrawModelDebug();
	}
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
	vsync = config->ReadBool("Vsync", true);
	SetVsync(vsync);
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
	config->WriteBool("Vsync", vsync);
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

void ModuleRenderer3D::SetVsync(bool value)
{
	vsync = value;
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
	vsync = true;
}

void ModuleRenderer3D::DrawModel() {

	list<geo_info> m_list = App->importer->GetModel();
	list<geo_info>::const_iterator m = m_list.begin();

	while (m != m_list.end())
	{
		//Model mesh
		glBindTexture(GL_TEXTURE_2D, ImgId);

		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, m->id_vertex);
		glVertexPointer(3, GL_FLOAT, 0, NULL);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, m->id_uv);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->id_index);

		
		glDrawElements(GL_TRIANGLES, m->num_index * 3, GL_UNSIGNED_INT, NULL);


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		glBindTexture(GL_TEXTURE_2D, 0);

		//Vertex normals
		if (draw_vert_normals) {
			glBegin(GL_LINES);
			glColor3f(255.0f, 255.0f, 0.0f); //Yellow
			for (int i = 0; i < m->num_normal * 3; i++) {

				glVertex3f(m->vertex[i], m->vertex[i + 1], m->vertex[i + 2]);
				glVertex3f(m->vertex[i] + m->normal[i], m->vertex[i + 1] + m->normal[i + 1], m->vertex[i + 2] + m->normal[i + 2]);

				i += 2;
			}
			glColor3f(255.0f, 255.0f, 255.0f); //White
			glEnd();
		}

		m++;
	}
}

void ModuleRenderer3D::DrawModelDebug()
{
	list<geo_debug> m_debug = App->importer->GetDebugInfo();
	list<geo_debug>::const_iterator deb = m_debug.begin();

	while (deb != m_debug.end())
	{
		//Face normals
		if (draw_face_normals) {

			glBegin(GL_LINES);

			glColor3f(255.0f, 0.0f, 0.0f); //Red

			for (int i = 0; i < deb->tri_normal.size(); i++) {

			glVertex3f(deb->tri_center[i].x, deb->tri_center[i].y, deb->tri_center[i].z);
			glVertex3f(deb->tri_center[i].x + deb->tri_normal[i].x, deb->tri_center[i].y + deb->tri_normal[i].y,
					   deb->tri_center[i].z + deb->tri_normal[i].z);

			}
			glColor3f(255.0f, 255.0f, 255.0f); //White

			glEnd();
		}

		deb++;
	}
}
