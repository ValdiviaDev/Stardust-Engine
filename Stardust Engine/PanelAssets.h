#ifndef __PANEL_ASSETS_H__
#define __PANEL_ASSETS_H__

#include "Panel.h"
#include <vector>
#include <map>

using namespace std;
typedef unsigned int UID;

class PanelAssets: public Panel
{
public:
	PanelAssets();
	virtual ~PanelAssets();

	void Draw();

	void GestionDirectoryTree(vector<string> dir);
	void DrawAssetTree(vector<string> files, string name, int& id, bool is_directory, bool is_mesh = false);

	void ImportFromAssets();
	void OpenScene();
	void PutOnGameObject();

	void SetMeshScenesMap(map<string, map<UID, string>> mesh_scenes);

private:
	//Focused
	int focused_node = -1;
	string foc_node_name;
	map<UID, string> foc_mesh;

	bool read_asset_changes = true;
	map<string, map<UID, string>> mesh_scenes;

};

#endif