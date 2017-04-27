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

void EngineState::handle(MouseEvent event)
{
}

void EngineState::handle(KeyEvent event)
{
	if (event.type == KeyEventType::KEY_RELEASED && event.key == KeyCodes::P)
	{
		switchEngineMode();
	}
}
