#include "Application.h"
#include "ModuleResourceManager.h"
#include "Resource.h"


ModuleResourceManager::ModuleResourceManager(Application* app, bool start_enabled) : Module(app, "Resources", start_enabled)
{
}


ModuleResourceManager::~ModuleResourceManager()
{
}

//Resource* ModuleResourceManager::CreateNewResource(ResourceType type, UID force_uid = 0)
//{
//	Resource* ret = nullptr;
//	//UID uid = GenerateNewUID();
//
//	switch (type) {
//	case Resource_Mesh: 
//		//ret = (Resource*)new ResourceMesh(uid);
//		break;
//	case Resource_Texture: 
//		//ret = (Resource*)new ResourceTexture(uid); 
//		break;
//
//
//	}
//
//	//if (ret != nullptr)
//	//	resources[uid] = ret;
//	
//	return ret;
//}

