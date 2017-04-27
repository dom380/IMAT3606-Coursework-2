#include "EngineState.h"

void EngineState::switchEngineMode()
{
	if (engineMode == EngineMode::GAME)
	{
		engineMode = EngineMode::EDITOR;
	}
	else
	{
		engineMode = EngineMode::GAME;
	}
}

EngineMode EngineState::getEngineMode()
{
	return engineMode;
}
