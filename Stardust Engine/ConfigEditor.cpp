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


void ConfigEditor::WriteBool(const char* name, bool value)
{
	LOG("Writing bool");
	json_object_set_boolean(module_object, name, value);
	json_serialize_to_file(root_value, "config.json");
}