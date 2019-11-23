#ifndef __MeshImporter__H__
#define __MeshImporter__H__

#include "Globals.h"
#include <string>
#include <list>
#include <vector>
#include <map>

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

class ComponentTransform;
class GameObject;
class ResourceMesh;

class MeshImporter
{
public:
	MeshImporter();
	~MeshImporter();

	bool ImportScene(const char* file, const char* path, std::string& output_file, std::vector<UID>& mesh_uids, UID parent_uid = 0, bool in_uids = false); //if in_uids, then we assign from outside the uids to the files we create inside
	bool ImportNodeAndSerialize(const aiScene* scene, const aiNode* node, GameObject* parent, ComponentTransform* transform, char* path, std::list<GameObject*>* go_list, std::vector<UID>& mesh_uuids, bool in_uids = false);
	bool SaveMesh(ResourceMesh* mesh, const char* file_name);
	bool LoadMesh(const char* exported_file, ResourceMesh* mesh);

	bool HasMeshOnlyTriangles(aiMesh* mesh);
	void ImportMatFromMesh(aiMaterial* material, GameObject* go);

private:
	std::map<aiMesh*, UID> charged_meshes;
	uint count_import = 0;
};

#endif