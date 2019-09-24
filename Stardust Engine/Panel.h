#ifndef __PANEL_H__
#define __PANEL_H__

class Panel
{
public:
	Panel();
	virtual ~Panel();

	void ToggleActive();
	bool IsActive();

	virtual void Draw() {};

protected:
	bool active = true;
	char* name = nullptr;
};

#endif