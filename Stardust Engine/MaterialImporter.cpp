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

bool MaterialImporter::Import(const char * file, const char * path, std::string & output_file)
{
	bool ret = false;


	std::string path_string(path);
	std::string file_string(file);

	std::string lib_tex = "Library/Textures/";


	if (IsFileDDS(file)) { //If file is already DDS just copy it to library
		LOG("File already DDS, copying to library");

		ret = App->fs->Copy((char*)(path_string + file_string).c_str(), (char*)(lib_tex + file_string).c_str());

	}
	else {


		char* buffer = nullptr;
		uint size = App->fs->Load((char*)(path_string + file_string).c_str(), &buffer);

		if (buffer) {


			//ILuint ImageName;				//Why does Ricard use this?
			//ilGenImages(1, &ImageName);
			//ilBindImage(ImageName);

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
						ret = App->fs->SaveUnique(output_file, data, size, lib_tex.c_str(), file_string.c_str(), "dds");

					}

					RELEASE_ARRAY(data);

				}
				//ilDeleteImages(1, &ImageName);
			}
		}
		RELEASE_ARRAY(buffer);

	}

	if (ret == false) {
		LOG("Can't load texture %s from path %s", file, path);
	}
	return ret;
}


bool MaterialImporter::LoadMaterial(const char * exported_file, ComponentMaterial * resource)
{
	return false;
}

bool MaterialImporter::LoadCheckers(ComponentMaterial * resource)
{
	return false;
}


bool MaterialImporter::IsFileDDS(const char* file_name) {

	bool ret = false;

	string extension = file_name;
	extension = extension.erase(NULL, extension.find_last_of("."));

	if (extension == ".dds" || extension == ".DDS")
		ret = true;

	return ret;
}
