#ifndef _2DE_LUA_UTILS_H_
#define _2DE_LUA_UTILS_H_

#include <lua.hpp>

#include "2de_Core.h"
#include "2de_Resource.h"

/**
* CScript - represents script resource type.
*/

class CScript : public CResource
{
public:
	bool Load();
	string GetScriptText() const;

private:
	string ScriptText;
};

/** 
* CLuaVirtualMachine - singleton, that allows access to common Lua virtual machine.
*
* Registers engine Lua API functions, and allows running Lua-scripts by filename, resource pointer or from string.
*/

class CLuaVirtualMachine : public CTSingleton<CLuaVirtualMachine>
{
public:
	bool RunFile(const string &AFilename);
	bool RunString(const string &AString);
	bool RunScript(CScript *AScript);
	void RegisterAPIFunction(const string &AName, lua_CFunction AFunc);

	bool CallFunction(const string &AFunctionName);
	bool CallMethodFunction(const string &AObjectName, const string &AFunctionName);
	void CreateLuaObject(const string &AName, CObject *AObject);

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
	friend class CLuaFunctionCall;
};

class CLuaFunctionCall
{
public:
	CLuaFunctionCall(const string &AFunctionName, int AResultsCount = 0);
	CLuaFunctionCall(const string &AObjectName, const string &AFunctionName, int AResultsCount = 0);
	~CLuaFunctionCall();

	bool Call();

	void PushArgument(lua_Number Argument);
	void PushArgument(const string &Argument);
	void PushArgument(void *Argument);

	template<typename T>
	T PopResult()
	{
		return PopResultImpl(identity<T>());
	}

private:
	template<typename T>
	T PopResultImpl(identity<T>)
	{
		if (Broken)
			return NULL;
		if (!Called)
		{
			Log("ERROR", "Can't pop a result from Lua function call: function has not been called yet");
			return NULL;
		}

		if (!ResultsCount)
		{
			Log("ERROR", "Can't pop a result from Lua function call: no more results");
			return NULL;
		}

		if (!lua_islightuserdata(State, -1))
		{
			Log("ERROR", "Can't pop a result from Lua function call: incorrect type");
			return NULL;
		}

		T res = static_cast<T>(lua_touserdata(State, -1));
		lua_pop(State, 1);
		ResultsCount--;
		return res;
	}

	string PopResultImpl(identity<string>)
	{
		if (Broken)
			return "";
		if (!Called)
		{
			Log("ERROR", "Can't pop a result from Lua function call: function has not been called yet");
			return "";
		}

		if (!ResultsCount)
		{
			Log("ERROR", "Can't pop a result from Lua function call: no more results");
			return "";
		}

		if (!lua_isstring(State, -1))
		{
			Log("ERROR", "Can't pop a result from Lua function call: incorrect type");
			return "";
		}

		string res = lua_tostring(State, -1);
		lua_pop(State, 1);
		ResultsCount--;
		return res;
	}

	lua_Number PopResultImpl(identity<lua_Number>)
	{
		if (Broken)
			return 0;
		if (!Called)
		{
			Log("ERROR", "Can't pop a result from Lua function call: function has not been called yet");
			return 0;
		}

		if (!ResultsCount)
		{
			Log("ERROR", "Can't pop a result from Lua function call: no more results");
			return 0;
		}

		if (!lua_isnumber(State, -1))
		{
			Log("ERROR", "Can't pop a result from Lua function call: incorrect type");
			return 0;
		}

		lua_Number res = lua_tonumber(State, -1);
		lua_pop(State, 1);
		ResultsCount--;
		return res;
	}

	string ObjectName;
	string FunctionName;

	int ArgumentsCount;
	int ResultsCount;
	bool Called;
	bool Broken;
	int OldStackTop;
	lua_State *State;
};

/**
* CScriptableComponent - component that is intended to make its parent scriptable.
*/

class CScriptableComponent : public CGameObject
{
public:
	CScriptableComponent(CScript *AScript);

	void ProcessEvent(const CEvent &AEvent);
};

#endif // _2DE_LUA_UTILS_H_
