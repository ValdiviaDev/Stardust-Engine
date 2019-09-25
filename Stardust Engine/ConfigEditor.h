#ifndef __CONFIG_EDITOR__
#define __CONFIG_EDITOR__

#include "Globals.h"

struct json_object_t;
typedef struct json_object_t JSON_Object;

struct json_value_t;
typedef struct json_value_t  JSON_Value;


class ConfigEditor {

public:

	ConfigEditor();
	~ConfigEditor();



private:

	JSON_Value* root_value = nullptr;
	JSON_Object* root_object = nullptr;

	
};




#endif // __CONFIG_EDITOR__