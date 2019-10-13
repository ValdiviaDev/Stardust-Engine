#include "PanelAbout.h"
#include "imgui/imgui.h"
#include "Application.h"
#include "Assimp/include/version.h"

#include "Glew/include/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>

//#include "DevIL/include/il.h"
//#include "DevIL/include/ilu.h"
//#include "DevIL/include/ilut.h"
#include "PhysFS/include/physfs.h"

PanelAbout::PanelAbout()
{
	name = "about";
	active = false;
	pos_x = 70, pos_y = 70, width = 500, height = 700;
}

PanelAbout::~PanelAbout()
{
}

void PanelAbout::Draw()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	//Set window position and size
	ImGui::SetNextWindowPos(ImVec2(pos_x, pos_y), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Once);

	//Window
	ImGui::Begin("About", &active, window_flags);
	ImGui::Text("Stardust Engine v0.3"); //TODO
	ImGui::Separator();

	//Description
	ImGui::Text("3D engine student project.");
	ImGui::Text("Made by Ricardo Gutierrez & David Valdivia.");
	ImGui::Text("\n");

	//3rd party libraries
	ImGui::Text("3rd party libraries used:");
	
	//SDL
	static SDL_version sdl_version;
	SDL_VERSION(&sdl_version); //Linked version
	ImGui::Text("	* SDL %i.%i.%i", sdl_version.major, sdl_version.minor, sdl_version.patch);

	//OpenGL
	static int gl_maj_ver, gl_min_ver;
	glGetIntegerv(GL_MAJOR_VERSION, &gl_maj_ver);
	glGetIntegerv(GL_MINOR_VERSION, &gl_min_ver);
	ImGui::Text("	* OpenGL %i.%i", gl_maj_ver, gl_min_ver);

	//Assimp
	static uint asmp_maj_ver = aiGetVersionMajor();
	static uint asmp_min_ver = aiGetVersionMinor();
	static uint asmp_rev_ver = aiGetVersionRevision();
	ImGui::Text("	* Assimp %i.%i.%i", asmp_maj_ver, asmp_min_ver, asmp_rev_ver);

	//DevIL
	ImGui::Text("	* DevIL 1.7.8"); //TODO

	//Others
	ImGui::Text("	* ImGui 1.72b");
	ImGui::Text("	* MathGeoLib 1.5");
	ImGui::Text("	* LGC RNG 1.5");
	ImGui::Text("	* Glew 2.1.0");

	//PhysFS
	static PHYSFS_Version fs_ver;
	PHYSFS_getLinkedVersion(&fs_ver);
	ImGui::Text("	* PhysFS %i.%i.%i", fs_ver.major, fs_ver.minor, fs_ver.patch);
	ImGui::Text("\n");

	//License
	ImGui::Text("License:");
	ImGui::Text("\n");
	ImGui::Text("MIT License");
	ImGui::Text("\n");
	ImGui::Text("Copyright (c) 2019 Ricardo Gutirrez & David Valdivia");
	ImGui::Text("\n");
	ImGui::Text("Permission is hereby granted, free of charge, to any person \n"
		"obtaining a copy of this software and associated documentation \n"
		"files(the 'Software'), to deal in the Software without restriction, \n"
		"including without limitation the rights to use, copy, modify, merge,\n"
		"publish, distribute, sublicense, and/or sell copies of the Software, \n"
		"and to permit persons to whom the Software is furnished to do so, \n"
		"subject to the following conditions : ");
	ImGui::Text("\n");
	ImGui::Text("The above copyright notice and this permission notice shall be \n"
		"included in all copies or substantial portions of the Software.");
	ImGui::Text("\n");
	ImGui::Text("THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, \n"
		"EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF \n"
		"MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.\n"
		"IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY \n"
		"CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, \n"
		"TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE \n"
		"SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");

	ImGui::End();
}
