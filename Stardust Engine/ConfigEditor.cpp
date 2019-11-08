#include "ConfigEditor.h"
#include "Parson/parson.h"



ConfigEditor::ConfigEditor(const char* file_name)
{
	root_value = json_parse_file(file_name);
	
	if (root_value == NULL) { 
		LOG("JSON not found, creating new file");
		root_value = json_value_init_object();
	}

	root_object = json_value_get_object(root_value);
	module_object = root_object;
}


ConfigEditor::ConfigEditor(const char* file_name, const char* module_name) {
	root_value = json_parse_file(file_name);

	if (root_value == NULL) {
		LOG("JSON not found, creating new file");
		root_value = json_value_init_object();
	}

	root_object = json_value_get_object(root_value);
	module_object = json_object_get_object(root_object, module_name);
}

ConfigEditor::~ConfigEditor()
{
	//Need to json_value_free
}


void ConfigEditor::SaveFile(const char* file_name) {
	json_serialize_to_file(root_value, file_name);
}


ConfigEditor ConfigEditor::GetModuleLocation(const char* name) {
	
	
	module_object = json_object_get_object(root_object, name);
	
	return *this;
}


void ConfigEditor::WriteBool(const char* name, bool value){
	LOG("Writing bool");
	json_object_set_boolean(module_object, name, value);
}


bool ConfigEditor::ReadBool(const char* name, bool default) const {

	bool ret = json_object_get_boolean(module_object, name);
	if (ret) {
		LOG("%s is true", name);
	}
	else {
		LOG("%s is false", name);
	}

	return ret;

	
	//return default;
}


void ConfigEditor::WriteInt(const char* name, int value) {
	LOG("Writing int");
	json_object_set_number(module_object, name, (double)value);
}


int ConfigEditor::ReadInt(const char* name, int default) const {
	LOG("Int is %i", (int)json_object_get_number(module_object, name));
	
	return (int)json_object_get_number(module_object, name);

	//return default;
}


void ConfigEditor::WriteUint(const char* name, uint value) {
	LOG("Writing uint %i", value);
	json_object_set_number(module_object, name, (double)value);

}


void ConfigEditor::WriteFloat(const char* name, float value) {
	LOG("Writing float");
	json_object_set_number(module_object, name, (float)value);
}


float ConfigEditor::ReadFloat(const char* name, float default) const {
	LOG("Float is %f", (float)json_object_get_number(module_object, name));

	return (float)json_object_get_number(module_object, name);

	//return default;
}


void ConfigEditor::WriteString(const char* name, const char* string) {
	LOG("Writing string");
	json_object_set_string(module_object, name, string);
}

	