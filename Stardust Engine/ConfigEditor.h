#ifndef __ConfigEditor__
#define __ConfigEditor__

#include "Globals.h"

struct json_object_t;
typedef struct json_object_t JSON_Object;

struct json_value_t;
typedef struct json_value_t  JSON_Value;

struct json_array_t;
typedef struct json_array_t  JSON_Array;


class ConfigEditor {

public:

	ConfigEditor(const char* file_name);
	ConfigEditor(const char* file_name, const char* module_name);
	~ConfigEditor();
	

	ConfigEditor GetModuleLocation(const char* name);

	void WriteBool(const char* name, bool value);
	bool ReadBool(const char* name, bool default) const;

	void WriteInt(const char* name, int value);
	int ReadInt(const char* name, int default) const;

	void WriteUint(const char* name, uint value);


	void WriteFloat(const char* name, float value);
	float ReadFloat(const char* name, float default) const;

	void WriteString(const char* name, const char* string);

	

	void SaveFile(const char* file_name);
public:

	JSON_Value* root_value = nullptr;
	JSON_Object* root_object = nullptr;
	JSON_Object* module_object = nullptr;


	
};




#endif // __CONFIG_EDITOR__