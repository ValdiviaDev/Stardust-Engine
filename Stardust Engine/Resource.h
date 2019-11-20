#ifndef __Resource_H__
#define __Resource_H__

#include "Globals.h"
#include <string>

enum ResourceType {
	Resource_Mesh,
	Resource_Texture,
	Resource_Unknown
};

class Resource
{
public:
	Resource(UID uuid, ResourceType type);
	virtual ~Resource();
	ResourceType GetType() const;
	UID GetUID() const;
	const char* GetFile() const;
	const char* GetImportedFile() const;
	bool IsLoadedToMemory() const;
	bool LoadToMemory();
	uint CountReferences() const;

	//virtual void Save(Config& config) const;
	//virtual void Load(const Config& config);
	virtual bool LoadInMemory() = 0;

protected:
	UID uuid = 0;
	std::string file;
	std::string imported_file;
	ResourceType type = Resource_Unknown;
	uint loaded = 0;

};

#endif