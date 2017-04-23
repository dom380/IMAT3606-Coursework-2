#ifndef NDEBUG
#include "..\..\include\Editor\DebugMenuItem.h"
#include <Engine.h>

DebugMenuItem::DebugMenuItem()
{
	id = Engine::g_pEngine->getDebugMenu()->getMenuItems().size();
	clearable = true;
	Engine::g_pEngine->getDebugMenu()->addMenuItem(this);
}

bool DebugMenuItem::canClear()
{
	return clearable;
}

#endif