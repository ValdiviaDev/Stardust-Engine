#ifndef __MeshImporter__H__
#define __MeshImporter__H__

#include "Globals.h"
#include <string>

struct geo_info;
struct aiScene;
struct aiNode;

class ComponentTransform;
class ComponentMesh;
class GameObject;

class MeshImporter
{
public:
	MeshImporter();
	~MeshImporter();

	bool ImportScene(const char* file, const char* path, std::string& output_file);
	bool Import(const void* buffer, uint size, std::string& output_file);
	bool ImportNode(const aiScene* scene, const aiNode* node, GameObject* parent, ComponentTransform* transform, char* path, std::string& output_file);
	bool SaveMesh(ComponentMesh* mesh, const char* file_name, std::string& output_file);

	bool LoadMesh(const char* exported_file, geo_info& mesh);

};

#endif