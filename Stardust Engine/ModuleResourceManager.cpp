#include "Application.h"
#include "ModuleResourceManager.h"
#include "Resource.h"


ModuleResourceManager::ModuleResourceManager(Application* app, bool start_enabled) : Module(app, "Resources", start_enabled)
{
}


ModuleResourceManager::~ModuleResourceManager()
{
}

UID ModuleResourceManager::ImportFile(const char * new_file_in_assets, ResourceType type)
{
	UID ret = 0;
	bool import_ok = false;
	string written_file;

	switch (type) {
	case Resource_Mesh:
		import_ok = App->mesh_import->ImportScene("", new_file_in_assets, written_file, true);
		break;
	case Resource_Texture:
		//import_ok = App->mat_import->Import(new_file_in_assets, "assets/tex", written_file, uid);
		break;
	}
	if (import_ok == true) { // If import was successful, create a new resource
		Resource* res = CreateNewResource(type);
		res->SetFile(new_file_in_assets);
		res->SetImportedFile(written_file);
		ret = res->GetUID();
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
	UID uid = GenerateNewUID();

	switch (type) {
	case Resource_Mesh:
		//ret = (Resource*)new ResourceMesh(uid);
		break;
	case Resource_Texture:
		//ret = (Resource*)new ResourceTexture(uid); 
		break;

	}

	if (ret != nullptr)
		resources[uid] = ret;

	return ret;
}

