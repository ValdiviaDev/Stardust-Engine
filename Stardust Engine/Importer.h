#ifndef __Importer__H__
#define __Importer__H__

#include "Application.h"

class Importer
{
public:
	Importer();
	virtual ~Importer();

	virtual bool Import(const char* file, const char* path, std::string& output_file) { return false; }
	virtual bool Import(const void* buffer, uint size, std::string& output_file) { return false; }


};

#endif