#include "PanelAssets.h"
#include "Application.h"
#include "imgui/imgui.h"

PanelAssets::PanelAssets()
{
	name = "assets";
	active = false;
	pos_x = 70, pos_y = 70, width = 500, height = 500;
}


PanelAssets::~PanelAssets()
{
	

}

void PanelAssets::Draw()
{
	//Set window position and size
	if (resize) {
		int x, y;
		App->window->GetWinSize(x, y);

		//Put in the middle of the screen. TODO change this later
		ImGui::SetNextWindowPos(ImVec2(x / 2 - (width / 2), y / 2 - (height / 2)), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
		resize = false;
	}


	static bool autom_scroll = true;

	//Window
	ImGui::Begin("Assets", &active, ImGuiWindowFlags_None);

	//Directory tree
	ImGui::BeginChild(1, { (float)width - 15, (float)height - 70 }, true);

	vector<string> file_list;
	vector<string> dir_list;

	App->fs->DiscoverFiles(ASSETS_FOLDER, file_list, dir_list);
	GestionDirectoryTree(dir_list);

	ImGui::EndChild();



	//ImGui::BeginChild(2, { (float)width - 15, (float)50 });
	ImGui::Button("Open", { 50, 20 });
	//ImGui::EndChild();


	ImGui::End();


}

void PanelAssets::GestionDirectoryTree(vector<string> dir)
{
	int id = 0;

	for (int i = 0; i < dir.size(); ++i) {
		vector<string> file_list;
		vector<string> dir_list;
		string this_dir = ASSETS_FOLDER + dir[i];

		App->fs->DiscoverFiles(this_dir.c_str(), file_list, dir_list);
		DrawAssetTree(file_list, dir[i], id, true);

		id++;
	}

}

void PanelAssets::DrawAssetTree(vector<string> files, string name, int id, bool is_directory)
{
	ImGui::PushID(id);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;

	if (!is_directory) //TODO change later
		flags |= ImGuiTreeNodeFlags_Leaf;

	if (ImGui::TreeNodeEx((void*)(intptr_t)id, flags, name.c_str())) {
	
		if (!is_directory) {
			if (ImGui::IsItemClicked()) {
				//TODO: TEMPORARY. Open file

			}
		}


		if (is_directory) {
			for (int i = 0; i < files.size(); ++i) {
				id++;
				DrawAssetTree(files, files[i], id, false);
			}
		}


		ImGui::TreePop();
	}

	ImGui::PopID();
}
