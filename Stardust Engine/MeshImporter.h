#ifndef __MeshImporter__H__
#define __MeshImporter__H__

#include "Globals.h"
#include <string>
#include <list>
#include <vector>

struct geo_info;
struct aiScene;
struct aiNode;

class ComponentTransform;
class GameObject;
class ResourceMesh;

class MeshImporter
{
public:
	MeshImporter();
	~MeshImporter();

	bool ImportScene(const char* file, const char* path, std::string& output_file, std::vector<UID>& mesh_uids);
	bool ImportNodeAndSerialize(const aiScene* scene, const aiNode* node, GameObject* parent, ComponentTransform* transform, char* path, std::list<GameObject*>* go_list, std::vector<UID>& mesh_uuids);
	bool SaveMesh(ResourceMesh* mesh, const char* file_name);
	bool LoadMesh(const char* exported_file, ResourceMesh* mesh);

};

#endif