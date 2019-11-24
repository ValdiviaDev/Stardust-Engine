#include "PanelAssets.h"
#include "Application.h"
#include "imgui/imgui.h"
#include "ModuleResourceManager.h"
#include "Resource.h"
#include "Globals.h"

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

	file_list.clear();
	dir_list.clear();
	App->fs->DiscoverFiles(ASSETS_FOLDER, file_list, dir_list);

	GestionDirectoryTree(dir_list);
	read_asset_changes = false;
	ImGui::EndChild();

	if (ImGui::Button("Open", { 80, 20 })) {
		OpenScene();
	}
	//ImGui::SameLine();
	//if (ImGui::Button("Import", { 80, 20 })) {
	//	ImportFromAssets();
	//}
	ImGui::SameLine();
	if (ImGui::Button("Put to GameObject", { 140, 20 })) {
		PutOnGameObject();
	}
	ImGui::SameLine();
	//if (ImGui::Button("Read Asset Changes", { 140, 20 })) {
	//	read_asset_changes = true;
	//}

	ImGui::End();
}

void PanelAssets::GestionDirectoryTree(vector<string> dir)
{
	int id = 0;

	for (int i = 0; i < dir.size(); ++i) {
		
		string this_dir = ASSETS_FOLDER + dir[i];

		vector<string> file_list;
		vector<string> dir_list;
		
		App->fs->DiscoverFiles(this_dir.c_str(), file_list, dir_list);
		
		if(dir[i] == "Meshes")
			DrawAssetTree(file_list, dir[i], id, true, File_Mesh);
		else if(dir[i] == "Textures")
			DrawAssetTree(file_list, dir[i], id, true, File_Material);
		else
			DrawAssetTree(file_list, dir[i], id, true, File_Unknown);

		id++;
	}
	
}

void PanelAssets::DrawAssetTree(vector<string> files, string name, int& id, bool is_directory, FileType ft)
{
	ImGui::PushID(id);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;

	if (!is_directory && ft != File_Mesh)
		flags |= ImGuiTreeNodeFlags_Leaf;

	if (id == focused_node)
		flags |= ImGuiTreeNodeFlags_Selected;

	if (ImGui::TreeNodeEx((void*)(intptr_t)id, flags, name.c_str())) {
	
		//File nodes
		if (!is_directory) {
			if (ImGui::IsItemClicked()) {
				focused_node = id;
				foc_node_name = name;
			}

			switch(ft) {
			case File_Mesh: {
				//Draw the mesh nodes inside de mesh scene (.fbx, .obj) node
				map<string, map<UID, string>>::iterator it = mesh_scenes.find(name);
				if (it != mesh_scenes.end()) {
					map<UID, string> mesh_scn = it->second;
					for (std::map<UID, string>::const_iterator it = mesh_scn.begin(); it != mesh_scn.end(); ++it) {
						id++;
						DrawAssetTree(files, it->second, id, false, No_Extension);

						//Get this mesh name and uuid
						if (ImGui::IsItemClicked())
							foc_mesh = mesh_scn;
					}
				}
			}
				break;
			case File_Material:
				//Get  this material uuid
				if (ImGui::IsItemClicked()) {
					map<string, UID>::iterator it = textures.find(name);
					if (it != textures.end())
						foc_tex_uuid = it->second;
				}
				break;
			}
		}

		//Directory nodes
		if (is_directory) {
			for (int i = 0; i < files.size(); ++i) {
				FileType ft = App->fs->DetermineFileType((char*)files[i].c_str());
				if (ft != File_Meta) { //Don't show metas
					id++;
					DrawAssetTree(files, files[i], id, false, ft);
				}
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
		string path = ASSETS_MESH_FOLDER + foc_node_name;
		App->scene_serialization->LoadSceneFromMesh(path.c_str());
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

void PanelAssets::PutOnGameObject()
{
	FileType ft = App->fs->DetermineFileType((char*)foc_node_name.c_str());
	switch (ft) {
	case No_Extension:
		for (std::map<UID, string>::const_iterator it = foc_mesh.begin(); it != foc_mesh.end(); ++it) {
			if (foc_node_name == it->second) {
				if (App->scene->GetFocusedGameObject())
					App->scene->AssignMeshToGameObject(it->first);
				else {
					GameObject* go = App->scene->CreateGameObjectByMesh(it->first);
					go->SetName(it->second.c_str());
				}
			}
		}
		break;
	case File_Material: {
		if (foc_tex_uuid != 0)
			App->scene->AssignTexToGameObject(foc_tex_uuid);
	}
	break;
	default:
		App->gui->AddLogToConsole("You cannot put this to a GameObject as a component");
		break;

	}
}

void PanelAssets::FillMeshScenesMap(map<string, map<UID, string>> mesh_scenes)
{
	this->mesh_scenes = mesh_scenes;
}

void PanelAssets::FillTexturesMap(map<string, UID> textures)
{
	this->textures = textures;
}