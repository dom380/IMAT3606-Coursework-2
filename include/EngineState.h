#ifndef ENGINESTATE_H
#define ENGINESTATE_H

enum EngineMode { EDITOR, GAME };


/*
	Controls the editor state of engine
*/
class EngineState
{
private:
	EngineMode engineMode = EngineMode::EDITOR;

public:
	EngineState() {};

	//Switch to other state
	void switchEngineMode();
	EngineMode getEngineMode();
};

#endif // !ENGINESTATE_H