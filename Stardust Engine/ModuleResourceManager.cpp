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

void ModuleResourceManager::GenerateMetaFile(const char* full_path, ResourceType type, UID uid, std::vector<UID>uids) {

	JSON_Value* root_value = json_value_init_object();
	JSON_Object* object = json_value_get_object(root_value);

	json_object_set_number(object, "UUID", uid);
	json_object_set_number(object, "Resource Type", type);


	JSON_Value* value_array = json_value_init_array();
	JSON_Array* array = json_value_get_array(value_array);

	if (!uids.empty()) {
		for (std::vector<UID>::const_iterator it = uids.begin(); it != uids.end(); it++) {

			JSON_Value* v = json_value_init_object();
			JSON_Object* obj = json_value_get_object(v);

			json_object_set_number(obj, "UUID", (*it));

			json_array_append_value(array, v);
		}

	}

	json_object_set_value(object, "Children", value_array);



	std::string path_meta = full_path;
	path_meta = path_meta + ".meta";

	json_serialize_to_file_pretty(root_value, path_meta.c_str());
	json_value_free(root_value);


	//JSON_Value* root_value = json_parse_file("Assets/test.rikarudo");
	//JSON_Object* object = json_value_get_object(root_value);

	switch (type) {
	case ResourceType::Resource_Mesh:

		break;

	case ResourceType::Resource_Texture:

		break;

	case ResourceType::Resource_Unknown:
		break;

	}
}