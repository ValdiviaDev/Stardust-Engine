#ifndef __SceneSerialization__
#define __SceneSerialization__

#include "Globals.h"
#include "Parson/parson.h"
#include "GameObject.h"


class SceneSerialization
{
public:

	SceneSerialization();
	~SceneSerialization();

	void SaveScene(const char* scene_name);


};




#endif