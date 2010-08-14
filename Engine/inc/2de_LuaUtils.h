#ifndef _2DE_LUA_UTILS_H_
#define _2DE_LUA_UTILS_H_

#include <lua.hpp>

#include "2de_Core.h"

/** 
* Класс CLuaVirtualMachine - синглтон, предоставляющий доступ к единой виртуальной машине Lua. Регистрирует Lua API функции движка, а также позволяет выполнять Lua-скрипты по имени файла.
*/

class CLuaVirtualMachine : public CTSingleton<CLuaVirtualMachine>
{
public:
	bool RunFile(const string &AFilename);
	void RegisterAPIFunction(const string &AName, lua_CFunction AFunc);

protected:
	CLuaVirtualMachine();
	~CLuaVirtualMachine();

private:
	void RegisterStandardAPI();

	lua_State *State;

	friend class CTSingleton<CLuaVirtualMachine>;
};

#endif // _2DE_LUA_UTILS_H_
