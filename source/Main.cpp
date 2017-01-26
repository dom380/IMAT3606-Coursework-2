#include "Engine.h"
#include <cstdlib>
#include <stdio.h>

Engine engine = Engine();
/*
	Program Entry point
*/
int main() {
	engine.init();
	engine.mainLoop();
	engine.exit();
	return 0;
}