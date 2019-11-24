#ifndef __ModuleResourceManager_H__
#define __ModuleResourceManager_H__

#include "Module.h"
#include <map>

class Resource;
enum ResourceType;

class ModuleResourceManager : public Module
{
public:
	ModuleResourceManager(Application* app, bool start_enabled = true);
	~ModuleResourceManager();

	bool Start();
	bool CleanUp();

	UID FindByFileInAssets(const char* file_in_assets) const; //ex. Assets/Meshes/Bakerhouse.fbx
	UID FindByFileInLibrary(const char* file_in_lib) const; //ex. Library/Meshes/31984913.stdtmesh
	std::map<UID, string> FindMeshes(const char* file_in_assets); //ex. Assets/Meshes/Bakerhouse.fbx
	UID ImportFile(const char* full_path, ResourceType type, UID parent_uid = 0, const std::vector<UID> childs_uids = vector<UID>()); //ex. Assets/Meshes/Bakerhouse.fbx
	UID GenerateNewUID();
	const Resource* Get(UID uid) const;
	Resource* Get(UID uid);
	Resource* CreateNewResource(ResourceType type, UID force_uid = 0, bool is_primitive = false);
	void GenerateMetaFile(const char* full_path, ResourceType type, UID uid, std::vector<UID>uids = std::vector<UID>()); //path = "Assets/Meshes/Baker_house.png"
	void CheckMeshMetas();
	void CheckTextureMetas();

	void GetAllMeshesFromScenes();
	void GetAllTexturesFromName();
	UID GetUIDFromMeta(std::string path_no_meta); // ex. Assets/Meshes/Bakerhouse.fbx
	
	uint GetResourcesCount() const;
	uint GetResourcesInMemory() const;

	void CreatePrimitiveResources();
	ResourceMesh* GetPrimitive(PrimitiveType type);

private:
	std::map<UID, Resource*> resources;

};

#endif