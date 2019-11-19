#ifndef __MaterialImporter__H__
#define __MaterialImporter__H__

#include "Globals.h"
#include <string>
#include <list>


class ComponentMaterial;

struct MatFileInfo {
	std::string file = "";
	uint uuid = 0;
	std::string path = "";

	MatFileInfo(const char* f, uint u, const char* p) {
		file = f;
		uuid = u;
		path = p;
	}
};

class MaterialImporter
{
public:
	MaterialImporter();
	~MaterialImporter();

	bool Import(const char* file, const char* path, std::string& output_file, uint& uid_out); //not necessary to pass uid_out if you dont need
	//bool Import(const void* buffer, uint size, std::string& output_file);

	bool LoadMaterial(const char* file_name, ComponentMaterial* mat);
	bool LoadCheckers(ComponentMaterial* resource);
	void Serialize(ComponentMaterial* mat);
	void SerializeNoComponent(const char* old_file, uint uuid); //file has to be for ex "BakerHouse.png"
	bool IsFileDDS(const char* file_name);
	uint AddTextureToList(const char* path, uint uuid);
	bool IsTextureLoaded(const char* path);
	const char* GetTexturePathFromUUID(uint uuid); //file has to be for ex "BakerHouse.png"
	uint GetUUIDFromJSON(const char* file);
	uint GetUUIDFromFile(const char* file); //file has to be for ex "BakerHouse.png"

public:
	std::list<MatFileInfo> loaded_tex_list;
};

#endif