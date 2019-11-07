#include "SceneSerialization.h"
#include "ModuleScene.h"
#include "Component.h"
#include "ConfigEditor.h"


SceneSerialization::SceneSerialization() {

}


SceneSerialization::~SceneSerialization() {

}



void SceneSerialization::SaveScene(const char* scene_name) {


	ConfigEditor config(scene_name);

	config.SaveFile(scene_name);

}
