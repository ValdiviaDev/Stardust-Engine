#ifndef __Panel_H__
#define __Panel_H__

class Panel
{
public:
	Panel();
	virtual ~Panel();

	void ToggleActive();
	bool IsActive() const;
	void ResizePanel();

	virtual void Draw() {};

protected:
	bool active = true;
	char* name = nullptr;
	int pos_x, pos_y, width, height;
	bool resize = true;
};

#endif