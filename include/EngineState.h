#ifndef ENGINESTATE_H
#define ENGINESTATE_H

enum EngineMode { EDITOR, GAME };


/*
	Controls the editor state of engine
*/
class EngineState
{
private:
	EngineMode engineMode;

public:
	EngineState() { engineMode = EngineMode::GAME; };

	//Switch to other state
	void switchEngineMode();
	void setEngineMode(EngineMode mode);
	EngineMode getEngineMode();
};

#endif // !ENGINESTATE_H