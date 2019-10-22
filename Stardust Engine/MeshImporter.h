#ifndef __MeshImporter__H__
#define __MeshImporter__H__

#include "Importer.h"

class ComponentMesh;

class MeshImporter : public Importer
{
public:
	MeshImporter();
	~MeshImporter();

	bool Import(const char* file, const char* path, std::string& output_file);
	bool Import(const void* buffer, uint size, std::string& output_file);

	bool Load(const char* exported_file, ComponentMesh* resource);

};

#endif