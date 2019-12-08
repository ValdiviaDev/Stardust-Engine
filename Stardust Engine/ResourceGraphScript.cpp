#include "Application.h"
#include "ResourceGraphScript.h"
#include "NodeGraph.h"



ResourceGraphScript::ResourceGraphScript(UID uuid) : Resource(uuid, Resource_Graph_Script)
{
}


ResourceGraphScript::~ResourceGraphScript()
{
}

bool ResourceGraphScript::LoadInMemory()
{
	node_graph = new NodeGraph();

	return true;
}

bool ResourceGraphScript::UnloadInMemory()
{
	RELEASE(node_graph);

	return true;
}
