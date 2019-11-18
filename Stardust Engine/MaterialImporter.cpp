#include "MaterialImporter.h"
#include "ComponentMaterial.h"
#include "Application.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")
#pragma comment (lib, "DevIL/libx86/DevIL.lib")
#pragma comment (lib, "DevIL/libx86/ILU.lib")
#pragma comment (lib, "DevIL/libx86/ILUT.lib")
#include "DevIL/include/il.h"
#include "DevIL/include/ilu.h"
#include "DevIL/include/ilut.h"


MaterialImporter::MaterialImporter()
{
}


MaterialImporter::~MaterialImporter()
{
}

bool MaterialImporter::Import(const char * file, const char * path, std::string & output_file, uint& uid_out)
{
	bool ret = false;

	std::string path_string(path);
	std::string file_string(file);

	uint uuid_mat = App->GenerateUUID();

	if (!IsTextureLoaded(file)) {

		//if (IsFileDDS(file)) { //If file is already DDS just copy it to library
		//	LOG("File already DDS, copying to library");

		//		ret = App->fs->Copy((char*)(path_string + file_string).c_str(), (char*)(LIBRARY_MAT_FOLDER + std::to_string(uuid_mat) + ".dds").c_str());

		//}
		//else {


			char* buffer = nullptr;
			uint size = App->fs->Load((char*)(path_string + file_string).c_str(), &buffer);

			if (buffer) {


				if (ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, size))
				{
					ilEnable(IL_FILE_OVERWRITE);


					ILuint size;
					ILubyte *data;

					ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);// To pick a specific DXT compression use
					size = ilSaveL(IL_DDS, NULL, 0); // Get the size of the data buffer
					if (size > 0) {
						data = new ILubyte[size]; // allocate data buffer
						if (ilSaveL(IL_DDS, data, size) > 0) // Save to buffer with the ilSaveIL function
						{
							file_string = file_string.substr(0, file_string.find('.', 0));
							ret = App->fs->SaveUnique(output_file, data, size, LIBRARY_MAT_FOLDER, std::to_string(uuid_mat).c_str(), "dds");

						}

						RELEASE_ARRAY(data);

					}
					
				}
			}
			RELEASE_ARRAY(buffer);

		//}

		if (ret) {
			LOG("Texture imported correctly: %s from path %s", file, path);
			string str = "Texture imported correctly: " + (string)file + " from path: " + path;
			App->gui->AddLogToConsole(str.c_str());

			string file_no_ext = file;
			//file_no_ext = file_no_ext.substr(0, file_no_ext.find_last_of("."));

			SerializeNoComponent(file, uuid_mat);
			AddTextureToList(file, uuid_mat);
			file_no_ext = std::to_string(uuid_mat);
			uid_out = uuid_mat;
			App->gui->loaded_materials.push_back((string)file_no_ext);


		}
		else {
			LOG("Can't import texture %s from path %s", file, path);
			string str = "Can't import texture: " + (string)file + " from path: " + path;
			App->gui->AddLogToConsole(str.c_str());
		}

	}

	return ret;
}


bool MaterialImporter::LoadMaterial(const char* file_name, ComponentMaterial* mat)
{
	string exported_file = LIBRARY_MAT_FOLDER + (string)file_name + ".dds";

	//Bind DevIL image
	uint image_id = 0;
	ilGenImages(1, &image_id);
	ilBindImage(image_id);

	string tex_str = "Loading texture: " + (string)exported_file;
	App->gui->AddLogToConsole(tex_str.c_str());

	//Load image
	if (ilLoad(IL_TYPE_UNKNOWN, exported_file.c_str())) {
		LOG("Texture Loaded Correctly");
		App->gui->AddLogToConsole("Texture Loaded Correctly");
	}
	else {
		LOG("Error: Couldn't load texture");
		App->gui->AddLogToConsole("Error: Couldn't load texture");
		ILenum Error;
		while ((Error = ilGetError()) != IL_NO_ERROR) {
			LOG("%d: %s", Error, iluErrorString(Error));
		}
		ilDeleteImages(1, &image_id);
		return false;
	}

	mat->uuid_mat = GetUUIDFromJSON(file_name);
	mat->SetPath(GetTexturePathFromUUID(mat->uuid_mat));

	//Get width and height
	mat->tex_width = ilGetInteger(IL_IMAGE_WIDTH);
	mat->tex_height = ilGetInteger(IL_IMAGE_HEIGHT);
	string size_str = "Texture size: " + std::to_string(mat->tex_width) + " x " + std::to_string(mat->tex_height);
	App->gui->AddLogToConsole(size_str.c_str());

	//Bind DevIL to OpenGL texture buffer
	mat->tex_id = ilutGLBindTexImage();
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);
	ilDeleteImages(1, &image_id);

	return true;
}

bool MaterialImporter::LoadCheckers(ComponentMaterial * resource)
{
	return false;
}

void MaterialImporter::Serialize(ComponentMaterial* mat)
{

	JSON_Value* root_value = json_value_init_array();
	JSON_Array* array = json_value_get_array(root_value);

	mat->Save(array);

	char folder_and_file[128];
	strcpy(folder_and_file, LIBRARY_MAT_FOLDER);
	strcat(folder_and_file, std::to_string(mat->uuid_mat).c_str());
	strcat(folder_and_file, ".json");

	json_serialize_to_file_pretty(root_value, folder_and_file);

	json_value_free(root_value);


}

void MaterialImporter::SerializeNoComponent(const char* old_file, uint uuid) {

	char folder_and_file[128];
	strcpy(folder_and_file, LIBRARY_MAT_FOLDER);
	strcat(folder_and_file, std::to_string(uuid).c_str());

	char path[128];
	strcpy(path, folder_and_file);

	strcat(folder_and_file, ".json");
	strcat(path, ".dds");


	JSON_Value* root_value = json_value_init_object();
	JSON_Object* object = json_value_get_object(root_value);

	json_object_set_number(object, "UUID", uuid);
	json_object_set_string(object, "old file", old_file);
	json_object_set_string(object, "path", path);


	json_serialize_to_file_pretty(root_value, folder_and_file);
	json_value_free(root_value);

}

bool MaterialImporter::IsFileDDS(const char* file_name) {

	bool ret = false;

	string extension = file_name;
	extension = extension.erase(NULL, extension.find_last_of("."));

	if (extension == ".dds" || extension == ".DDS")
		ret = true;

	return ret;
}


uint MaterialImporter::AddTextureToList(const char* path, uint uuid) {


	

	for (std::list<MatFileInfo>::const_iterator it = loaded_tex_list.begin(); it != loaded_tex_list.end(); it++) {

		if (it->file == path) {

			return it->uuid;
		}

	}

	char p[128];
	strcpy(p, LIBRARY_MAT_FOLDER);//CHECK IF .png.dds
	string name = path;
	name = name.substr(0, name.find_last_of("."));
	
	strcat(p, path);
	strcat(p, ".dds");
	MatFileInfo aux(path, uuid, p);
	loaded_tex_list.push_back(aux);

	return uuid;
}

bool MaterialImporter::IsTextureLoaded(const char* path) {

	for (std::list<MatFileInfo>::const_iterator it = loaded_tex_list.begin(); it != loaded_tex_list.end(); it++) {
		//if (strcmp(it->path.c_str(), path)) {
		if (it->file == path) {
	
			LOG("Material Importer: Texture already loaded on list");
			return true;
		}
	}

	return false;
}



const char* MaterialImporter::GetTexturePathFromUUID(uint uuid) {

	for (std::list<MatFileInfo>::const_iterator it = loaded_tex_list.begin(); it != loaded_tex_list.end(); it++) {
		
		if (it->uuid == uuid) {

			return it->path.c_str();
		}
	}

	return "";
}

uint MaterialImporter::GetUUIDFromJSON(const char * file)
{
	
	/*char p[100];
	strcpy(p, LIBRARY_MAT_FOLDER);
	strcat(p, file);
	strcat(p, ".json");*/
	
	
	JSON_Value* root_value = json_parse_file(file);
	JSON_Object* object = json_value_get_object(root_value);

	uint aux = json_object_get_number(object, "UUID");

	return aux;

}
