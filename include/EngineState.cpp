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

void EngineState::setEngineMode(EngineMode mode)
{
	engineMode = mode;
}

EngineMode EngineState::getEngineMode()
{
	return engineMode;
}
