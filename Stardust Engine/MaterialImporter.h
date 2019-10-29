#ifndef __MaterialImporter__H__
#define __MaterialImporter__H__

#include "Globals.h"
#include <string>

class ComponentMaterial;

class MaterialImporter
{
public:
	MaterialImporter();
	~MaterialImporter();

	bool Import(const char* file, const char* path, std::string& output_file);
	//bool Import(const void* buffer, uint size, std::string& output_file);

	bool LoadMaterial(const char* file_name, ComponentMaterial* mat);
	bool LoadCheckers(ComponentMaterial* resource);

	bool IsFileDDS(const char* file_name);
};

#endif