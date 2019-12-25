#include "Application.h"
#include "ResourceGraphScript.h"
#include "NodeGraph.h"
#include "ModuleResourceManager.h"



ResourceGraphScript::ResourceGraphScript(UID uuid) : Resource(uuid, Resource_Graph_Script)
{

	
}


ResourceGraphScript::~ResourceGraphScript()
{

}

bool ResourceGraphScript::LoadInMemory()
{
	node_graph = new NodeGraph();

	node_graph->LoadFile(uuid, App->resources->names[uuid]);

	return true;
}

bool ResourceGraphScript::UnloadInMemory()
{
	RELEASE(node_graph);

	return true;
}
