#include "Engine.h"
#include <cstdlib>
#include <stdio.h>

shared_ptr<Engine> Engine::g_pEngine = std::make_shared<Engine>();
/*
	Program Entry point
*/
int main() {
	Engine::g_pEngine->init();
	Engine::g_pEngine->mainLoop();
	Engine::g_pEngine->exit();
	return 0;
}