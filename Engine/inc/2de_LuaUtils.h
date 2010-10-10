#ifndef _2DE_LUA_UTILS_H_
#define _2DE_LUA_UTILS_H_

#include <lua.hpp>

#include "2de_Core.h"

/** 
* CLuaVirtualMachine - синглтон, предоставляющий доступ к единой виртуальной машине Lua.
*
* Регистрирует Lua API функции движка, а также позволяет выполнять Lua-скрипты по имени файла.
*/

class CLuaVirtualMachine : public CTSingleton<CLuaVirtualMachine>
{
public:
	bool RunFile(const string &AFilename);
	bool RunString(const string &AString);
	void RegisterAPIFunction(const string &AName, lua_CFunction AFunc);

	int GetMemoryUsage() const;
	void RunGC();

	string GetLastError() const;
	void TriggerError(const string &AMessage, ...);

protected:
	CLuaVirtualMachine();
	~CLuaVirtualMachine();

private:
	void RegisterStandardAPI();

	lua_State *State;

	string LastError;

	friend class CTSingleton<CLuaVirtualMachine>;
};

#endif // _2DE_LUA_UTILS_H_
