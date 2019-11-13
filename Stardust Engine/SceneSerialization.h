#ifndef __SceneSerialization__
#define __SceneSerialization__

#include "Globals.h"
#include "Parson/parson.h"
#include "GameObject.h"
#include <list>


struct json_object_t;
typedef struct json_object_t JSON_Object;

struct json_value_t;
typedef struct json_value_t  JSON_Value;

struct json_array_t;
typedef struct json_array_t  JSON_Array;

class SceneSerialization
{
public:

	SceneSerialization();
	~SceneSerialization();

	void SaveScene(const char* scene_name);
	void LoadScene(const char* scene_name);
	void SceneSerialization::SaveGOs(GameObject* go, JSON_Array* array); //Send root first time

	void SaveSceneFromMesh(const char* scene_name, std::list<GameObject*> go_list);
};




#endif