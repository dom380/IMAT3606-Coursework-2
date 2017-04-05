#include "..\..\include\Editor\DebugMenuItem.h"
#include <Engine.h>
DebugMenuItem::DebugMenuItem()
{
	Engine::g_pEngine->getDebugMenu()->addMenuItem(this);
}

void DebugMenuItem::debugMenuItemUpdate()
{
}
