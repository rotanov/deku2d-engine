#ifndef _LUA_CONSOLE_H_
#define _LUA_CONSOLE_H_

#include "2de_Engine.h"

class CLuaConsole : public CObject
{
public:
	CLuaConsole();
	bool InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter);
	void Write(const string &AText);
	void WriteLine(const string &AText);

private:
	void CleanUp();
	void WritePrompt();
	void Input(const string &AText);
	CEdit *CommandLineEdit;
	CText *CommandOutput;
	list<string> CommandHistory;
	list<string>::iterator CommandHistoryIterator;
	int MaxLines;
	int CurLines;

};


#endif // _LUA_CONSOLE_H_
