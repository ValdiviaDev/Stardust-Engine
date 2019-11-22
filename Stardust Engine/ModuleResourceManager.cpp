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

bool ModuleResourceManager::Start() {

	bool ret = true;

	CheckMetas();
	CreatePrimitiveResources();

	return ret;
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
				written_file = LIBRARY_MESH_FOLDER + std::to_string(mesh_uuids[i]) + "." + MESH_EXTENSION;
				res->SetImportedFile(written_file);
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
			res->SetImportedFile(written_file);
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




	switch (type) {
	case ResourceType::Resource_Mesh: {

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



		break;
	}
	case ResourceType::Resource_Texture:

		break;

	case ResourceType::Resource_Unknown:
		break;

	}
}


void ModuleResourceManager::CheckMetas() {

	//Get all files in the folder
	std::vector<std::string> files, dirs;
	App->fs->DiscoverFiles(ASSETS_MESH_FOLDER, files, dirs);

	
	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); it++) {

		//Look if they are .meta
		std::string file = ASSETS_MESH_FOLDER + (*it);
		FileType ft = App->fs->DetermineFileType(file.c_str());

		if (ft == FileType::File_Meta) {

			
			JSON_Value* root_value = json_parse_file(file.c_str());
			JSON_Object* object = json_value_get_object(root_value);

			UID file_uid = json_object_get_number(object, "UUID");
			int r_type = json_object_get_number(object, "Resource Type");


			//Look if the Library file is created
			std::string file_lib = LIBRARY_MESH_FOLDER + std::to_string(file_uid) + "." + MESH_EXTENSION;

			if (App->fs->Exists(file_lib.c_str())) {
				//If created, do resource only
				CreateNewResource((ResourceType)r_type, file_uid);

			}
			else {
				//If not created, import
				file = file.substr(0, file.find_last_of("."));
				ImportFile(file.c_str(), (ResourceType)r_type);
			}


			json_value_free(root_value);

		}
	}
}



//Primitives
void ModuleResourceManager::CreatePrimitiveResources()
{
	ResourceMesh* primitive[3];

	for (int i = 0; i < 3; ++i) {
		primitive[i] = (ResourceMesh*)CreateNewResource(Resource_Mesh);
		primitive[i]->SetFile("null");
		primitive[i]->SetImportedFile("null");
	}
	primitive[0]->is_primitive = PRIMITIVE_CUBE;
	primitive[1]->is_primitive = PRIMITIVE_SPHERE;
	primitive[2]->is_primitive = PRIMITIVE_PLANE;
}

ResourceMesh* ModuleResourceManager::GetPrimitive(PrimitiveType type)
{
	for (std::map<UID, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it) {
		ResourceMesh* aux = (ResourceMesh*)it->second;
		
		if (aux->is_primitive == type)
			return (ResourceMesh*)it->second;
	}

	return nullptr;
}

