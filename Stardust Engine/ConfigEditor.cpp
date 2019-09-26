#include "ConfigEditor.h"
#include "Parson/parson.h"



ConfigEditor::ConfigEditor()
{
	root_value = json_parse_file("config.json");
	
	if (root_value == NULL) { 
		LOG("JSON not found, creating new file");
		root_value = json_value_init_object();
	}

	root_object = json_value_get_object(root_value);
	module_object = root_object;
}


ConfigEditor::ConfigEditor(const char* module_name) {
	root_value = json_parse_file("config.json");

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

ConfigEditor ConfigEditor::GetModuleLocation(const char* name) {
	
	
	module_object = json_object_get_object(root_object, name);
	
	return *this;


}


void ConfigEditor::WriteBool(const char* name, bool value){
	LOG("Writing bool");
	json_object_set_boolean(module_object, name, value);
	json_serialize_to_file(root_value, "config.json"); //MIRAR SI ESTO SE TIENE QE HACER CADA VZ O UNA VEZ al fiNAL Y YA
}


bool ConfigEditor::ReadBool(const char* name, bool default) const {

	if(json_object_get_boolean(module_object, name) == true)
		LOG("%s is true", name);
	if (json_object_get_boolean(module_object, name) == false)
		LOG("%s is false", name);

	return json_object_get_boolean(module_object, name);

	
	//return default;
}


void ConfigEditor::WriteInt(const char* name, int value) {
	LOG("Writing int");
	json_object_set_number(module_object, name, (double)value);
	json_serialize_to_file(root_value, "config.json");
}


int ConfigEditor::ReadInt(const char* name, int default) const {
	LOG("Int is %i", (int)json_object_get_number(module_object, name));
	
	return (int)json_object_get_number(module_object, name);

	//return default;
}


void ConfigEditor::WriteFloat(const char* name, float value) {
	LOG("Writing float");
	json_object_set_number(module_object, name, (float)value);
	json_serialize_to_file(root_value, "config.json");
}


float ConfigEditor::ReadFloat(const char* name, float default) const {
	LOG("Float is %f", (float)json_object_get_number(module_object, name));

	return (float)json_object_get_number(module_object, name);

	//return default;
}