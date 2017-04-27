#ifndef ENGINESTATE_H
#define ENGINESTATE_H

#include <GUI\EventListener.h>

enum EngineMode { EDITOR, GAME };


/*
	Controls the editor state of engine
*/
class EngineState : public EventListener
{
private:
	EngineMode engineMode = EngineMode::EDITOR;

public:
	EngineState() {};

	//Switch to other state
	void switchEngineMode();
	EngineMode getEngineMode();

	//Event Listener methods
	/*
	stub method
	*/
	void handle(MouseEvent event);
	/*
	Pauses and unpauses the game on 'P' key press.
	*/
	void handle(KeyEvent event);
};

#endif // !ENGINESTATE_H