#include "2de_LuaUtils.h"

#include "2de_Engine.h"

/**
* LuaAPI - пространство имён для функций, вызываемых из Lua-скриптов. Самим движком эти функции не используются.
*/

namespace LuaAPI
{
	// часть из этих функций действительно останется в API, другая - просто временные для теста..

	// void Log(string Event, string Text)
	int WriteToLog(lua_State *L)
	{
		::Log(lua_tostring(L, -2), lua_tostring(L, -1));
		return 0;
	}

	// bool IsSpacePressed()
	int IsSpacePressed(lua_State *L)
	{
		lua_pushboolean(L, (::CEngine::Instance()->keys[SDLK_SPACE]) ? true : false);
		return 1;
	}

	// number GetDeltaTime()
	int GetDeltaTime(lua_State *L)
	{
		lua_pushnumber(L, ::CEngine::Instance()->GetDeltaTime());
		return 1;
	}

	// example of using light user data objects
	
	// userdata CreateNewText(string Name)
	int CreateNewText(lua_State *L)
	{
		::CText *text = ::CFactory::Instance()->New< ::CText>(lua_tostring(L, -1));

		lua_pushlightuserdata(L, text);

		return 1;
	}

	// void SetText(userdata TextObject, string NewText)
	int SetText(lua_State *L)
	{
		::CText *text = static_cast< ::CText *>(lua_touserdata(L, -2));
		if (!text)
			::Log("LUAERROR", "Incorrect usage of light user data in SetText API call");
		else
			text->SetText(lua_tostring(L, -1));

		return 0;
	}

	// void SetPosition(userdata RenderableObject, number X, number Y)
	int SetPosition(lua_State *L)
	{
		::CRenderable *obj = static_cast< ::CRenderable *>(lua_touserdata(L, -3));
		if (!obj)
			::Log("LUAERROR", "Incorrect usage of light user data in SetPosition API call");
		else
			obj->Position = Vector2(lua_tonumber(L, -2), lua_tonumber(L, -1));

		return 0;
	}

};

//////////////////////////////////////////////////////////////////////////
// CLuaVirtualMachine

CLuaVirtualMachine::CLuaVirtualMachine()
{
	State = luaL_newstate();
	if (!State)
	{
		Log("ERROR", "Lua state initialization failed");
		return;
	}

	RegisterStandardAPI();
}

CLuaVirtualMachine::~CLuaVirtualMachine()
{
	lua_close(State);
}

bool CLuaVirtualMachine::RunFile(const string &AFilename)
{
	if (!State)
		return false;

	return (luaL_dofile(State, AFilename.c_str()) == 0);
}

void CLuaVirtualMachine::RegisterAPIFunction(const string &AName, lua_CFunction AFunc)
{
	if (!State)
		return;

	lua_register(State, AName.c_str(), AFunc);
}

int CLuaVirtualMachine::GetMemoryUsage() const
{
	if (!State)
		return 0;

	return lua_gc(State, LUA_GCCOUNT, 0);
}

void CLuaVirtualMachine::RunGC()
{
	if (!State)
		return;

	lua_gc(State, LUA_GCCOLLECT, 0);
}

void CLuaVirtualMachine::RegisterStandardAPI()
{
	lua_register(State, "Log", &LuaAPI::WriteToLog);
	lua_register(State, "GetDeltaTime", &LuaAPI::GetDeltaTime);
	lua_register(State, "IsSpacePressed", &LuaAPI::IsSpacePressed);
	lua_register(State, "CreateNewText", &LuaAPI::CreateNewText);
	lua_register(State, "SetText", &LuaAPI::SetText);
	lua_register(State, "SetPosition", &LuaAPI::SetPosition);
}


// маленький помощник, чтобы самим не считать количество lua_push...() и lua_pop()
// 	пока не юзается, но может понадобится, так что оставлю..
/*class LuaStackGuard
{
public:
	LuaStackGuard(lua_State* L) : luaState_(L)
	{
		top_ = lua_gettop(L);
	}

	~LuaStackGuard()
	{
		lua_settop(luaState_, top_);
	}

private:
	lua_State* luaState_;
	int top_;
};*/

