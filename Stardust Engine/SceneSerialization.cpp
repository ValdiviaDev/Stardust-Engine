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

void SceneSerialization::SaveGOs(GameObject* go, JSON_Array* array) {

	

	if (go->GetNumChilds() > 0) {

		for (std::vector<GameObject*>::const_iterator it = go->childs.begin(); it < go->childs.end(); it++) {


			(*it)->Save(array);
			SaveGOs(*it, array);
		}
	}


	

}