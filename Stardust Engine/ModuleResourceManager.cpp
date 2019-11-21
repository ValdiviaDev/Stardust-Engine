#include "Application.h"
#include "ModuleResourceManager.h"
#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"


ModuleResourceManager::ModuleResourceManager(Application* app, bool start_enabled) : Module(app, "Resources", start_enabled)
{
}


ModuleResourceManager::~ModuleResourceManager()
{
}

bool ModuleResourceManager::CleanUp()
{
	//Clean all resources
	for (std::map<UID, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
		RELEASE(it->second);

	resources.clear();

	return true;
}

UID ModuleResourceManager::ImportFile(const char* new_file_in_assets, ResourceType type)
{
	UID ret = 0;
	string written_file;
	std::string path = "", file = "", aux = "", path_and_file = "";
	App->fs->SplitFilePath(new_file_in_assets, &path, &file, &aux);

	switch (type) {
	case Resource_Mesh: {
		vector<UID> mesh_uuids;
		if (App->mesh_import->ImportScene(file.c_str(), new_file_in_assets, written_file, mesh_uuids)) {
			for (int i = 0; i < mesh_uuids.size(); ++i) {
				Resource* res = CreateNewResource(type, mesh_uuids[i]);
				res->SetFile(new_file_in_assets);
				res->SetImportedFile(written_file); //TODO
				ret = res->GetUID();
			}
		}
	}
		break;
	case Resource_Texture: {
		UID tex_uuid;
		if (App->mat_import->Import(file.c_str(), path.c_str(), written_file, tex_uuid)) {
			Resource* res = CreateNewResource(type, tex_uuid);
			res->SetFile(new_file_in_assets);
			res->SetImportedFile(written_file);  //TODO
			ret = res->GetUID();

		}
	}
		break;
	}
	
	return ret;	
}

UID ModuleResourceManager::GenerateNewUID()
{
	return App->GenerateUUID(); //TODO
}

Resource* ModuleResourceManager::Get(UID uid)
{
	std::map<UID, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
		return it->second;
	else
		return nullptr;
}

Resource* ModuleResourceManager::CreateNewResource(ResourceType type, UID force_uid)
{
	Resource* ret = nullptr;
	UID uid = 0;

	if (force_uid != 0)
		uid = force_uid;
	else
		uid = GenerateNewUID();

	switch (type) {
	case Resource_Mesh:
		ret = (Resource*)new ResourceMesh(uid);
		break;
	case Resource_Texture:
		ret = (Resource*)new ResourceTexture(uid); 
		break;

	}

	if (ret != nullptr)
		resources[uid] = ret;

	return ret;
}

