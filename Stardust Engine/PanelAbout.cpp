#include "PanelAbout.h"
#include "imgui/imgui.h"

PanelAbout::PanelAbout()
{
	name = "about";
	active = false;
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
	ImGui::SetNextWindowPos({ 70,70 }, ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(500, 700), ImGuiCond_Once);

	//Window
	ImGui::Begin("About", &active, window_flags);
	ImGui::Text("Stardust Engine v0.1"); //TODO
	ImGui::Separator();
	ImGui::Text("3D engine student project.");
	ImGui::Text("Made by Ricardo Gutierrez & David Valdivia.");
	ImGui::Text("\n");
	ImGui::Text("3rd party libraries used:");
	ImGui::Text("	* SDL "); //TODO
	ImGui::Text("	* ImGui "); //TODO
	ImGui::Text("	* MathGeoLib "); //TODO
	ImGui::Text("	* LGC RNG "); //TODO
	ImGui::Text("	* Glew "); //TODO
	ImGui::Text("\n");

	//License
	ImGui::Text("License:");
	ImGui::Text("\n");
	ImGui::Text("MIT License");
	ImGui::Text("\n");
	ImGui::Text("Copyright (c) 2019 Ricardo Gutiérrez & David Valdivia");
	ImGui::Text("\n");
	ImGui::Text("Permission is hereby granted, free of charge, to any person \nobtaining a copy of this software and associated documentation \nfiles(the 'Software'), to deal in the Software without restriction, \nincluding without limitation the rights to use, copy, modify, merge,\npublish, distribute, sublicense, and/or sell copies of the Software, \nand to permit persons to whom the Software is furnished to do so, \nsubject to the following conditions : ");
	ImGui::Text("\n");
	ImGui::Text("The above copyright notice and this permission notice shall be \nincluded in all copies or substantial portions of the Software.");
	ImGui::Text("\n");
	ImGui::Text("THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, \nEXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF \nMERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.\nIN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY \nCLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, \nTORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE \nSOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");

	ImGui::End();
}
