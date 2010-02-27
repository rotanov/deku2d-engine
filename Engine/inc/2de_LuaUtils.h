#ifndef _2DE_LUA_UTILS_H
#define _2DE_LUA_UTILS_H

#include <lua.hpp>

#include "2de_Core.h"

//using namespace std;

struct lua_State;
class GlobalLuaState
{
public:
	struct Exception : public std::exception
	{
		//Exception(const std::string& what) : std::exception(what.c_str())
		Exception(const std::string& what) : std::exception()
		{
		}
	};

	GlobalLuaState(const std::string& filename);
	~GlobalLuaState();

	// функции, определенные внутри скрипта filename
	void outerFunction1();
	int outerFunction2();
	std::string outerFunction3();

	// функции, которые могут быть вызваны из скрипта filename
	void innerFunction1();
	int innerFunction2();
	std::string innerFunction3();

protected:
	lua_State* L_;
};

// кто-то где-то должен создать этот указатель
extern GlobalLuaState* globalLuaState;

//////////////////////////////////////////////////////////////////////////

class CLuaTest
{
public:
	CLuaTest(){}
};


#endif // _2DE_LUA_UTILS_H
