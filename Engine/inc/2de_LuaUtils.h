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
