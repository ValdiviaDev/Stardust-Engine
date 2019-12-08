#ifndef __ResourceGraphScript_H__
#define __ResourceGraphScript_H__

#include "Resource.h"

class NodeGraph;

class ResourceGraphScript : public Resource
{
public:
	ResourceGraphScript(UID uuid);
	~ResourceGraphScript();

	bool LoadInMemory();
	bool UnloadInMemory();

public:
	NodeGraph* node_graph;

};

#endif