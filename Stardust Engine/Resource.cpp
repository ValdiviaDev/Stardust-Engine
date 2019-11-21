#include "Resource.h"


Resource::Resource(UID uuid, ResourceType type): uuid(uuid), type(type)
{
}

Resource::~Resource()
{
}

ResourceType Resource::GetType() const
{
	return type;
}

UID Resource::GetUID() const
{
	return uuid;
}

const char* Resource::GetFile() const
{
	return file.c_str();
}

const char* Resource::GetImportedFile() const
{
	return imported_file.c_str();
}

bool Resource::IsLoadedToMemory() const
{
	if (loaded == 0)
		return false;
	else
		return true;
}

bool Resource::LoadToMemory() //TODO change maybe
{
	bool ret = true;
	if (!IsLoadedToMemory())
		ret = LoadInMemory();

	loaded++;

	return ret;
}

bool Resource::EraseToMemory() //TODO change maybe
{
	bool ret = true;
	if (loaded > 0)
		loaded--;

	if (loaded == 0)
		ret = EraseInMemory();

	return ret;
}

uint Resource::CountReferences() const
{
	return loaded;
}

void Resource::SetFile(std::string file)
{
	this->file = file;
}

void Resource::SetImportedFile(std::string imported_file)
{
	this->imported_file = imported_file;
}


