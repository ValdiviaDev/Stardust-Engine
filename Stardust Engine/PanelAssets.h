#ifndef __PANEL_ASSETS_H__
#define __PANEL_ASSETS_H__

#include "Panel.h"
#include <vector>

using namespace std;

class PanelAssets: public Panel
{
public:
	PanelAssets();
	virtual ~PanelAssets();

	void Draw();

	void GestionDirectoryTree(vector<string> dir);
	void DrawAssetTree(vector<string> files, string name, int id, bool is_directory);

private:

};

#endif