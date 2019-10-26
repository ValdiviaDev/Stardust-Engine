#ifndef __MeshImporter__H__
#define __MeshImporter__H__

#include "Globals.h"
#include <string>

struct geo_info;

class MeshImporter
{
public:
	MeshImporter();
	~MeshImporter();

	bool Import(const char* file, const char* path, std::string& output_file);
	bool Import(const void* buffer, uint size, std::string& output_file);

	bool LoadMesh(const char* exported_file, geo_info& mesh);

};

#endif