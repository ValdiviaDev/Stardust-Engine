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

	std::string lib_tex = LIBRARY_MAT_FOLDER;


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

	if (ret) {
		LOG("Texture imported correctly: %s from path %s", file, path);
		string str = "Texture imported correctly: " + (string)file + " from path: " + path;
		App->gui->AddLogToConsole(str.c_str());

		string file_no_ext = file;
		file_no_ext = file_no_ext.substr(0, file_no_ext.find_last_of("."));
		App->gui->loaded_materials.push_back((string)file_no_ext);
	}
	else {
		LOG("Can't import texture %s from path %s", file, path);
		string str = "Can't import texture: " + (string)file + " from path: " + path;
		App->gui->AddLogToConsole(str.c_str());
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


bool MaterialImporter::IsFileDDS(const char* file_name) {

	bool ret = false;

	string extension = file_name;
	extension = extension.erase(NULL, extension.find_last_of("."));

	if (extension == ".dds" || extension == ".DDS")
		ret = true;

	return ret;
}
