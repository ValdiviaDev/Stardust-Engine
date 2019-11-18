#ifndef __MaterialImporter__H__
#define __MaterialImporter__H__

#include "Globals.h"
#include <string>
#include <list>


class ComponentMaterial;

struct MatFileInfo {
	std::string path = "";
	uint uuid = 0;

	MatFileInfo(const char* p, uint u) {
		path = p;
		uuid = u;
	}
};

class MaterialImporter
{
public:
	MaterialImporter();
	~MaterialImporter();

	bool Import(const char* file, const char* path, std::string& output_file);
	//bool Import(const void* buffer, uint size, std::string& output_file);

	bool LoadMaterial(const char* file_name, ComponentMaterial* mat);
	bool LoadCheckers(ComponentMaterial* resource);
	void Serialize(ComponentMaterial* mat);
	void SerializeNoComponent(const char* old_file, uint uuid); //file has to be for ex "BakerHouse.png"
	bool IsFileDDS(const char* file_name);
	uint AddTextureToList(const char* path, uint uuid);
	bool IsTextureLoaded(const char* path);

public:
	std::list<MatFileInfo> loaded_tex_list;
};

#endif