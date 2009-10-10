#ifndef  _LUA_UTILS_H_
#define _LUA_UTILS_H_

#include "2de_Core.h"
#include "lua.hpp"
//using namespace std;

struct lua_State;
class GlobalLuaState
{
public:
	struct Exception : public std::exception
	{
		Exception(const std::string& what) : std::exception(what.c_str())
		{
		}
	};

	GlobalLuaState(const std::string& filename);
	~GlobalLuaState();

	// �������, ������������ ������ ������� filename
	void outerFunction1();
	int outerFunction2();
	std::string outerFunction3();

	// �������, ������� ����� ���� ������� �� ������� filename
	void innerFunction1();
	int innerFunction2();
	std::string innerFunction3();

protected:
	lua_State* L_;
};

// ���-�� ���-�� ������ ������� ���� ���������
extern GlobalLuaState* globalLuaState;

//////////////////////////////////////////////////////////////////////////

class CLuaTest
{
public:
	CLuaTest(){}
};


#endif
