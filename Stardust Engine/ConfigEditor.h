#ifndef __CONFIG_EDITOR__
#define __CONFIG_EDITOR__

#include "Globals.h"

struct json_object_t;
typedef struct json_object_t JSON_Object;

struct json_value_t;
typedef struct json_value_t  JSON_Value;

struct json_array_t;
typedef struct json_array_t  JSON_Array;


class ConfigEditor {

public:

	ConfigEditor();
	ConfigEditor(const char* module_name);
	~ConfigEditor();
	

	ConfigEditor GetModuleLocation(const char* name);

	void WriteBool(const char* name, bool value);
	bool ReadBool(const char* name, bool default) const;

	void WriteInt(const char* name, int value);
	int ReadInt(const char* name, int default) const;


private:

	JSON_Value* root_value = nullptr;
	JSON_Object* root_object = nullptr;
	JSON_Object* module_object = nullptr;

	
};




#endif // __CONFIG_EDITOR__