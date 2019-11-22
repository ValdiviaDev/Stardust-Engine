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

	bool CleanUp();

	UID Find(const char* file_in_assets) const; //TODO
	UID ImportFile(const char* new_file_in_assets, ResourceType type);
	UID GenerateNewUID(); //TODO
	const Resource* Get(UID uid) const; //TODO
	Resource* Get(UID uid);
	Resource* CreateNewResource(ResourceType type, UID force_uid = 0);
	void GenerateMetaFile(const char* full_path, ResourceType type, UID uid, std::vector<UID>uids = std::vector<UID>()); //path = "Assets/Meshes/Baker_house.png"

private:
	std::map<UID, Resource*> resources;

};

#endif