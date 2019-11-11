#include "SceneSerialization.h"
#include "ModuleScene.h"
#include "Component.h"
#include "ConfigEditor.h"
#include "Application.h"

SceneSerialization::SceneSerialization() {

}


SceneSerialization::~SceneSerialization() {

}



void SceneSerialization::SaveScene(const char* scene_name) {

	LOG("Saving scene %s.", scene_name)

	JSON_Value* root_value = json_value_init_array();

	//root_value = json_parse_file("scene1.json");

	//if (root_value == NULL) {
	//	LOG("JSON not found, creating new file");
	//	root_value = json_value_init_object();
	//}

	JSON_Array* array = json_value_get_array(root_value);

	SaveGOs(App->scene->GetRootGameObject(),array);


	
	json_serialize_to_file_pretty(root_value, scene_name);


	json_value_free(root_value);
}



void SceneSerialization::LoadScene(const char* scene_name) {

	LOG("Loading scene %s.", scene_name);

	App->scene->DestroyGOs();

	JSON_Value* root_value = json_parse_file(scene_name);
	JSON_Array* array = json_value_get_array(root_value);
	JSON_Object* object;

	std::vector<GameObject*>go_list;
	int array_size = json_array_get_count(array);
	go_list.reserve(array_size);

	App->scene->CreateRootObject();
	

	for (uint i = 0; i < array_size; i++) {

		object = json_array_get_object(array, i);
		GameObject* aux = App->scene->CreateGameObject(App->scene->GetRootGameObject());
		aux->SetName(json_object_get_string(object, "Name"));

		go_list.push_back(aux);
	}
	


}



void SceneSerialization::SaveGOs(GameObject* go, JSON_Array* array) {

	

	if (go->GetNumChilds() > 0) {

		for (std::vector<GameObject*>::const_iterator it = go->childs.begin(); it < go->childs.end(); it++) {


			(*it)->Save(array);
			SaveGOs(*it, array);
		}
	}


	

}