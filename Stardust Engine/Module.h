#pragma once

class Application;
class ConfigEditor;

class Module
{
private :
	bool enabled;
	char name[20];

public:
	Application* App;

	Module(Application* parent, const char* name, bool start_enabled = true) : App(parent)
	{
		strcpy_s(this->name, 20, name);
	}

	virtual ~Module()
	{}

	virtual bool Init(ConfigEditor* config) 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	const char* GetName() const
	{
		return name;
	}

	virtual void Save(ConfigEditor* config) const
	{}

	virtual void Load(ConfigEditor* config)
	{}
};