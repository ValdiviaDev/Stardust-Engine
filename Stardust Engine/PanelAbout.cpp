#include "PanelAbout.h"
#include "imgui/imgui.h"

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
	ImGui::Text("Stardust Engine v0.1"); //TODO
	ImGui::Separator();
	ImGui::Text("3D engine student project.");
	ImGui::Text("Made by Ricardo Gutierrez & David Valdivia.");
	ImGui::Text("\n");
	ImGui::Text("3rd party libraries used:");
	ImGui::Text("	* SDL 2.0.4"); //TODO
	ImGui::Text("	* ImGui 1.72b"); //TODO
	ImGui::Text("	* MathGeoLib 1.5"); //TODO
	ImGui::Text("	* LGC RNG 1.5"); //TODO
	ImGui::Text("	* Glew 2.1.0"); //TODO
	ImGui::Text("\n");

	//License
	ImGui::Text("License:");
	ImGui::Text("\n");
	ImGui::Text("MIT License");
	ImGui::Text("\n");
	ImGui::Text("Copyright (c) 2019 Ricardo Gutirrez & David Valdivia");
	ImGui::Text("\n");
	ImGui::Text("Permission is hereby granted, free of charge, to any person \nobtaining a copy of this software and associated documentation \nfiles(the 'Software'), to deal in the Software without restriction, \nincluding without limitation the rights to use, copy, modify, merge,\npublish, distribute, sublicense, and/or sell copies of the Software, \nand to permit persons to whom the Software is furnished to do so, \nsubject to the following conditions : ");
	ImGui::Text("\n");
	ImGui::Text("The above copyright notice and this permission notice shall be \nincluded in all copies or substantial portions of the Software.");
	ImGui::Text("\n");
	ImGui::Text("THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, \nEXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF \nMERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.\nIN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY \nCLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, \nTORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE \nSOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");

	ImGui::End();
}
