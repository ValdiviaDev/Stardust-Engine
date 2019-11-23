#include "PanelAssets.h"
#include "Application.h"
#include "imgui/imgui.h"
#include "ModuleResourceManager.h"
#include "Resource.h"

PanelAssets::PanelAssets()
{
	name = "assets";
	active = true;
	pos_x = 70, pos_y = 70, width = 300, height = 500;
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
		height = y - 550;
		ImGui::SetNextWindowPos(ImVec2(0.0f,  y - height), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
		resize = false;
	}


	//Window
	ImGui::Begin("Assets", &active, ImGuiWindowFlags_None);

	//Directory tree
	ImGui::BeginChild(1, { (float)width - 15, (float)height - 70 }, true);

	vector<string> file_list;
	vector<string> dir_list;

	App->fs->DiscoverFiles(ASSETS_FOLDER, file_list, dir_list);
	GestionDirectoryTree(dir_list);

	ImGui::EndChild();

	if (ImGui::Button("Open", { 80, 20 })) {
		OpenScene();
	}
	//ImGui::SameLine();
	//if (ImGui::Button("Import", { 80, 20 })) {
	//	ImportFromAssets();
	//}

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

void PanelAssets::DrawAssetTree(vector<string> files, string name, int& id, bool is_directory)
{
	ImGui::PushID(id);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;

	if (!is_directory) //TODO change later
		flags |= ImGuiTreeNodeFlags_Leaf;

	if (id == focused_node)
		flags |= ImGuiTreeNodeFlags_Selected;

	if (ImGui::TreeNodeEx((void*)(intptr_t)id, flags, name.c_str())) {
	
		if (!is_directory) {
			if (ImGui::IsItemClicked()) {
				focused_node = id;
				foc_node_name = name;
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

void PanelAssets::ImportFromAssets()
{
	FileType ft = App->fs->DetermineFileType((char*)foc_node_name.c_str());
	switch (ft) {
	case File_Mesh: { //TODO import
		string path = ASSETS_MESH_FOLDER + foc_node_name;
		App->resources->ImportFile(path.c_str(), Resource_Mesh);
		}
		break;

	case File_Material: { //TODO import
		string path = ASSETS_TEX_FOLDER + foc_node_name;
		App->resources->ImportFile(path.c_str(), Resource_Texture);
		}
		break;

	case File_Scene:
	default:
		App->gui->AddLogToConsole("ERROR: Couldn't import file!!");
		break;

	}
}

void PanelAssets::OpenScene()
{
	FileType ft = App->fs->DetermineFileType((char*)foc_node_name.c_str());
	switch (ft) {
	case File_Mesh: {
		std::string path = "", file = "", aux = "", path_and_file = "";
		App->fs->SplitFilePath(foc_node_name.c_str(), &path, &file, &aux);
		path_and_file = LIBRARY_SCENE_FOLDER + file + ".json";

		App->scene_serialization->LoadScene(path_and_file.c_str());
	}

		break;

	case File_Scene: {
		string path = ASSETS_SCENE_FOLDER + foc_node_name;
		App->scene_serialization->LoadScene(path.c_str());
	}
		break;
	
	default:
		App->gui->AddLogToConsole("You can't charge this file type into the scene");
		break;

	}
}
