#include "Panel.h"

Panel::Panel()
{
}

Panel::~Panel()
{
}

void Panel::ToggleActive()
{
	active = !active;
}

bool Panel::IsActive()
{
	return active;
}