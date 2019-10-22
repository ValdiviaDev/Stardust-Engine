#ifndef __MaterialImporter__H__
#define __MaterialImporter__H__

#include "Importer.h"

class ComponentMaterial;

class MaterialImporter : public Importer
{
public:
	MaterialImporter();
	~MaterialImporter();

	bool Import(const char* file, const char* path, std::string& output_file);
	bool Import(const void* buffer, uint size, std::string& output_file);

	bool Load(const char* exported_file, ComponentMaterial* resource);
	bool LoadCheckers(ComponentMaterial* resource);

};

#endif