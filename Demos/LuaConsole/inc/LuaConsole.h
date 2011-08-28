#ifndef _LUA_CONSOLE_H_
#define _LUA_CONSOLE_H_

#include "2de_Engine.h"
using namespace Deku2D;

class CLuaConsole : public CObject
{
public:
	CLuaConsole();
	void ProcessEvent(const CEvent &AEvent);
	void Write(const string &AText);
	void WriteLine(const string &AText);

private:
	void CleanUp();
	void WritePrompt();
	void Input(const string &AText);
	CGameObject *CommandLineEdit;
	CText *CommandOutput;
	list<string> CommandHistory;
	list<string>::iterator CommandHistoryIterator;
	int MaxLines;
	int CurLines;

};


#endif // _LUA_CONSOLE_H_
