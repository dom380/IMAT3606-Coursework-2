#pragma once
#ifndef KEYEVENTTYPE_H
#define KEYEVENTTYPE_H

/*
	Enum specifying the type of Key Event.
	KEY_PRESSED == Initial/One press.
	KEY_REPEATED == Key held down.
	KEY_RELEASED == Key released.
*/
enum class KeyEventType
{
	KEY_PRESSED,
	KEY_REPEATED,
	KEY_RELEASED
};

#endif // !KEYEVENTTYPE_H

