#include "2de_LuaUtils.h"

#include "2de_Engine.h"

/**
* LuaAPI - namespace for functions that is called from Lua scripts. The engine itself doesn't use them.
*/

namespace LuaAPI
{
	// some of these functions are really will be left in the API, others are just temporarily ones for testing..

	// userdata GetObject(string Name)
	int GetObject(lua_State *L)
	{
		if (!lua_isstring(L, -1))
			CLuaVirtualMachine::Instance()->TriggerError("incorrect arguments given to GetObject API call");

		lua_pushlightuserdata(L, CFactory::Instance()->Get<CObject>(lua_tostring(L, -1)));
		return 1;
	}

	// string GetName(userdata Object)
	int GetName(lua_State *L)
	{
		CObject *obj = static_cast<CObject *>(lua_touserdata(L, -1));
		if (!obj)
			CLuaVirtualMachine::Instance()->TriggerError("incorrect usage of light user data in GetName API call");

		lua_pushstring(L, obj->GetName().c_str());
		return 1;
	}

	// void Attach(component Destination, component Object)
	int Attach(lua_State *L)
	{
		CGameObject *GameObjectSource = NULL, *GameObjectDestination = NULL;
		GameObjectSource = static_cast<CGameObject *>(lua_touserdata(L, -2));
		if (!GameObjectSource)
			CLuaVirtualMachine::Instance()->TriggerError("incorrect usage of light user data in Attach API call");

		GameObjectDestination = static_cast<CGameObject *>(lua_touserdata(L, -1));
		if (!GameObjectDestination)
			CLuaVirtualMachine::Instance()->TriggerError("incorrect usage of light user data in Attach API call");

		GameObjectSource->Attach(GameObjectDestination);
		return 0;
	}

	// void Log(string Event, string Text)
	int WriteToLog(lua_State *L)
	{
		if (!lua_isstring(L, -1) || !lua_isstring(L, -2))
			CLuaVirtualMachine::Instance()->TriggerError("incorrect arguments given to Log API call");

		::Log(lua_tostring(L, -2), lua_tostring(L, -1));
		return 0;
	}

	// number GetDeltaTime()
	int GetDeltaTime(lua_State *L)
	{
		lua_pushnumber(L, CEngine::Instance()->GetDeltaTime());
		return 1;
	}

	// userdata CreateNewText(string Name)
	int CreateNewText(lua_State *L)
	{
		if (!lua_isstring(L, -1))
			CLuaVirtualMachine::Instance()->TriggerError("incorrect arguments given to CreateNewText API call");

		CText *text = CFactory::Instance()->New<CText>(lua_tostring(L, -1));
		lua_pushlightuserdata(L, text);
		return 1;
	}

	// string GetText(userdata TextObject)
	int GetText(lua_State *L)
	{
		CText *text = static_cast< ::CText *>(lua_touserdata(L, -1));
		if (!text)
			CLuaVirtualMachine::Instance()->TriggerError("incorrect usage of light user data in GetText API call");

		lua_pushstring(L, text->GetText().c_str());
		return 1;
	}

	// void SetText(userdata TextObject, string NewText)
	int SetText(lua_State *L)
	{
		CText *text = static_cast<CText *>(lua_touserdata(L, -2));
		if (!text)
			CLuaVirtualMachine::Instance()->TriggerError("incorrect usage of light user data in SetText API call");

		if (!lua_isstring(L, -1))
			CLuaVirtualMachine::Instance()->TriggerError("incorrect arguments given to SetText API call");

		text->SetText(lua_tostring(L, -1));

		return 0;
	}

	// void SetTextFont(userdata TextObject, userdata FontObject)
	int SetTextFont(lua_State *L)
	{
		CText *text = static_cast<CText *>(lua_touserdata(L, -2));
		if (!text)
			CLuaVirtualMachine::Instance()->TriggerError("incorrect usage of light user data in SetTextFont API call");
	
		CFont *font = static_cast<CFont *>(lua_touserdata(L, -1));
		if (!font)
			CLuaVirtualMachine::Instance()->TriggerError("incorrect usage of light user data in SetTextFont API call");

		text->SetFont(font);
		return 0;
	}

	// userdata GetFont(string Name)
	int GetFont(lua_State *L)
	{
		if (!lua_isstring(L, -1))
			CLuaVirtualMachine::Instance()->TriggerError("incorrect arguments given to GetFont API call");

		CFont *font = CFactory::Instance()->Get<CFont>(lua_tostring(L, -1));
		lua_pushlightuserdata(L, font);
		return 1;
	}

	// (number, number) GetPosition(userdata RenderableComponent)
	int GetPosition(lua_State *L)
	{
		CRenderableComponent *rcobj = static_cast<CRenderableComponent *>(lua_touserdata(L, -1));
		if (!rcobj)
		{
			CLuaVirtualMachine::Instance()->TriggerError("incorrect usage of light user data in GetPosition API call");
		}

		lua_pushnumber(L, rcobj->GetPosition().x);
		lua_pushnumber(L, rcobj->GetPosition().y);
		return 2;
	}

	// void SetPosition(userdata RenderableComponent, number X, number Y)
	int SetPosition(lua_State *L)
	{
		if (!lua_isnumber(L, -1) || !lua_isnumber(L, -2))
			CLuaVirtualMachine::Instance()->TriggerError("incorrect arguments given to SetPosition API call");

		CObject* cobj = static_cast<CObject*>(lua_touserdata(L, -3));
		CRenderableComponent *rcobj = static_cast<CRenderableComponent *>(cobj);
		if (!rcobj)
		{
			CLuaVirtualMachine::Instance()->TriggerError("incorrect usage of light user data in SetPosition API call");
			return 0;
		}

		rcobj->SetPosition(Vector2(lua_tonumber(L, -2), lua_tonumber(L, -1)));
		return 0;
	}

	// number GetWindowWidth()
	int GetWindowWidth(lua_State *L)
	{
		lua_pushnumber(L, CGLWindow::Instance()->GetWidth());
		return 1;
	}

	// number GetWindowHeight()
	int GetWindowHeight(lua_State *L)
	{
		lua_pushnumber(L, CGLWindow::Instance()->GetHeight());
		return 1;
	}

	// (number, number) GetWindowDimensions()
	int GetWindowDimensions(lua_State *L)
	{
		lua_pushnumber(L, CGLWindow::Instance()->GetWidth());
		lua_pushnumber(L, CGLWindow::Instance()->GetHeight());
		return 2;
	}

	// void SubscribeToEvent(string EventName, userdata Object)
	int SubscribeToEvent(lua_State *L)
	{
		if (!lua_isstring(L, -2))
			CLuaVirtualMachine::Instance()->TriggerError("incorrect arguments given to SubscribeToEvent API call");

		CObject *obj = static_cast<CObject *>(lua_touserdata(L, -1));
		if (!obj)
			CLuaVirtualMachine::Instance()->TriggerError("incorrect usage of light user data in SubscribeToEvent API call");

		CEventManager::Instance()->Subscribe(lua_tostring(L, -2), obj);

		return 0;
	}

	// void UnsubscribeFromEvent(string EventName, userdata Object)
	int UnsubscribeFromEvent(lua_State *L)
	{
		if (!lua_isstring(L, -2))
			CLuaVirtualMachine::Instance()->TriggerError("incorrect arguments given to UnsubscribeFromEvent API call");

		CObject *obj = static_cast<CObject *>(lua_touserdata(L, -1));
		if (!obj)
			CLuaVirtualMachine::Instance()->TriggerError("incorrect usage of light user data in UnsubscribeFromEvent API call");

		CEventManager::Instance()->Unsubscribe(lua_tostring(L, -2), obj);

		return 0;
	}

	// void TriggerEvent(string EventName, userdata Sender) // the first implementation, without parameters support...
	int TriggerEvent(lua_State *L)
	{
		if (!lua_isstring(L, -2))
			CLuaVirtualMachine::Instance()->TriggerError("incorrect arguments given to TriggerEvent API call");

		CObject *obj = static_cast<CObject *>(lua_touserdata(L, -1));
		if (!obj)
			CLuaVirtualMachine::Instance()->TriggerError("incorrect usage of light user data in TriggerEvent API call");

		CEventManager::Instance()->TriggerEvent(lua_tostring(L, -2), obj);

		return 0;
	}

	// userdata GetEventSender(userdata Event)
	int GetEventSender(lua_State *L)
	{
		CEvent *obj = static_cast<CEvent *>(lua_touserdata(L, -1));
		if (!obj)
			CLuaVirtualMachine::Instance()->TriggerError("incorrect usage of light user data in GetEventSender API call");

		lua_pushlightuserdata(L, obj->GetSender());
		return 1;
	}

	// string GetEventData(userdata Event, string DataName)
	int GetEventData(lua_State *L)
	{
		if (!lua_isstring(L, -1))
			CLuaVirtualMachine::Instance()->TriggerError("incorrect arguments given to GetEventData API call");

		CEvent *obj = static_cast<CEvent *>(lua_touserdata(L, -2));
		if (!obj)
			CLuaVirtualMachine::Instance()->TriggerError("incorrect usage of light user data in GetEventData API call");

		lua_pushstring(L, obj->GetData<string>(lua_tostring(L, -1)).c_str());
		return 1;
	}

	// number sin(number n)
	int sin(lua_State *L)
	{
		if (!lua_isnumber(L, -1))
			CLuaVirtualMachine::Instance()->TriggerError("incorrect arguments given to sin API call");

		lua_pushnumber(L, ::sin(lua_tonumber(L, -1)));
		return 1;
	}

	// number cos(number n)
	int cos(lua_State *L)
	{
		if (!lua_isnumber(L, -1))
			CLuaVirtualMachine::Instance()->TriggerError("incorrect arguments given to cos API call");

		lua_pushnumber(L, ::cos(lua_tonumber(L, -1)));
		return 1;
	}

	// number Abs(number n)
	int Abs(lua_State *L)
	{
		if (!lua_isnumber(L, -1))
			CLuaVirtualMachine::Instance()->TriggerError("incorrect arguments given to Abs API call");

		lua_pushnumber(L, ::Abs(lua_tonumber(L, -1)));
		return 1;
	}

	// number Random_Int(number min, number max)
	int Random_Int(lua_State *L)
	{
		if (!lua_isnumber(L, -1) || !lua_isnumber(L, -2))
			CLuaVirtualMachine::Instance()->TriggerError("incorrect arguments given to Random_Int API call");

		lua_pushnumber(L, ::Random_Int(lua_tonumber(L, -2), lua_tonumber(L, -1)));
		return 1;
	}

	// number Random_Float(number min, number max)
	int Random_Float(lua_State *L)
	{
		if (!lua_isnumber(L, -1) || !lua_isnumber(L, -2))
			CLuaVirtualMachine::Instance()->TriggerError("incorrect arguments given to Random_Float API call");

		lua_pushnumber(L, ::Random_Float(lua_tonumber(L, -2), lua_tonumber(L, -1)));
		return 1;
	}

	// userdata GetParent(userdata Object)
	int GetParent(lua_State *L)
	{
		CGameObject *obj = static_cast<CGameObject *>(lua_touserdata(L, -1));
		if (!obj)
			CLuaVirtualMachine::Instance()->TriggerError("incorrect usage of light user data in GetParent API call");

		lua_pushlightuserdata(L, obj->Parent);
		return 1;
	}

	// number GetMemoryUsage()
	int GetMemoryUsage(lua_State *L)
	{
		lua_pushnumber(L, CLuaVirtualMachine::Instance()->GetMemoryUsage());
		return 1;
	}

};

//////////////////////////////////////////////////////////////////////////
// CScript

bool CScript::Load()
{
	if (Loaded)
		return true;

	if (Source == LOAD_SOURCE_FILE)
	{
		if (!CFileSystem::Exists(Filename))
		{
			Log("ERROR", "Can't load script from a file named '%s': file doesn't exist", Filename.c_str());
			return false;
		}
	}
	else if (Source == LOAD_SOURCE_MEMORY)
	{
		CMemory mem(MemoryLoadData, MemoryLoadLength, CStorage::OPEN_MODE_READ);
		ScriptText = mem.GetContent();
	}

	CResource::Load();

	return true;
}

string CScript::GetScriptText() const
{
	return ScriptText;
}

//////////////////////////////////////////////////////////////////////////
// CLuaVirtualMachine

CLuaVirtualMachine::CLuaVirtualMachine()
{
	SetName("CLuaVirtualMachine");

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

	if (luaL_dofile(State, AFilename.c_str()) !=0)
	{
		LastError = lua_tostring(State, -1);
		Log("ERROR", "Lua: %s", LastError.c_str());
		return false;
	}

	return true;
}

bool CLuaVirtualMachine::RunString(const string &AString)
{
	if (!State)
		return false;
	
	if (luaL_dostring(State, AString.c_str()) != 0)
	{
		LastError = lua_tostring(State, -1);
		Log("ERROR", "Lua: %s", LastError.c_str());
		return false;
	}

	return true;
}

bool CLuaVirtualMachine::RunScript(CScript *AScript)
{
	if (!State)
		return false;

	if (AScript == NULL)
	{
		Log("ERROR", "Script is NULL in CLuaVirtualMachine::RunScript");
		return false;
	}
	
	if (AScript->GetLoadSource() == CResource::LOAD_SOURCE_FILE)
	{
		return RunFile(AScript->GetFilename());
	}
	else if (AScript->GetLoadSource() == CResource::LOAD_SOURCE_MEMORY)
	{
		return RunString(AScript->GetScriptText());
	}

	// can't happen
	return false;
}

void CLuaVirtualMachine::RegisterAPIFunction(const string &AName, lua_CFunction AFunc)
{
	if (!State)
		return;

	lua_register(State, AName.c_str(), AFunc);
}

bool CLuaVirtualMachine::CallFunction(const string &AFunctionName)
{
	return CLuaFunctionCall(AFunctionName).Call();
}

bool CLuaVirtualMachine::CallMethodFunction(const string &AObjectName, const string &AFunctionName)
{
	return CLuaFunctionCall(AObjectName, AFunctionName).Call();
}

/**
* CLuaVirtualMachine::CreateLuaObject - creates a new Lua object, puts it in the global namespace table under AName name and sets its "object" field to AObject.
*/

void CLuaVirtualMachine::CreateLuaObject(const string &AName, CObject *AObject)
{
	// Note: object must already exist on Lua side. ObjectName = { } in any Lua file will suffice.

	lua_getglobal(State, AName.c_str());
	if (lua_isnil(State, -1))
	{
		Log("ERROR", "An error occured while creating '%s': object must be defined in Lua script first", AName.c_str());
		lua_pop(State, 1);
		return;
	}

	lua_pushlightuserdata(State, AObject);
	lua_setfield(State, -2, "object");
	lua_pop(State, 1);
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

string CLuaVirtualMachine::GetLastError() const
{
	return LastError;
}

void CLuaVirtualMachine::TriggerError(const string &AMessage, ...)
{
	if (!State)
		return;
	
	va_list args;
	va_start(args, AMessage);
	luaL_error(State, AMessage.c_str(), args);
	va_end(args);
}

void CLuaVirtualMachine::RegisterStandardAPI()
{
	luaopen_base(State);

	lua_register(State, "GetObject", &LuaAPI::GetObject);
	lua_register(State, "GetName", &LuaAPI::GetName);
	lua_register(State, "Attach", &LuaAPI::Attach);
	lua_register(State, "Log", &LuaAPI::WriteToLog);
	lua_register(State, "GetDeltaTime", &LuaAPI::GetDeltaTime);
	lua_register(State, "CreateNewText", &LuaAPI::CreateNewText);
	lua_register(State, "SetText", &LuaAPI::SetText);
	lua_register(State, "GetText", &LuaAPI::GetText);
	lua_register(State, "SetTextFont", &LuaAPI::SetTextFont);
	lua_register(State, "GetFont", &LuaAPI::GetFont);
	lua_register(State, "GetPosition", &LuaAPI::GetPosition);
	lua_register(State, "SetPosition", &LuaAPI::SetPosition);
	lua_register(State, "GetWindowWidth", &LuaAPI::GetWindowWidth);
	lua_register(State, "GetWindowHeight", &LuaAPI::GetWindowHeight);
	lua_register(State, "GetWindowDimensions", &LuaAPI::GetWindowDimensions);
	lua_register(State, "SubscribeToEvent", &LuaAPI::SubscribeToEvent);
	lua_register(State, "UnsubscribeFromEvent", &LuaAPI::UnsubscribeFromEvent);
	lua_register(State, "TriggerEvent", &LuaAPI::TriggerEvent);
	lua_register(State, "GetEventSender", &LuaAPI::GetEventSender);
	lua_register(State, "GetEventData", &LuaAPI::GetEventData);
	lua_register(State, "sin", &LuaAPI::sin);
	lua_register(State, "cos", &LuaAPI::cos);
	lua_register(State, "Abs", &LuaAPI::Abs);
	lua_register(State, "Random_Int", &LuaAPI::Random_Int);
	lua_register(State, "Random_Float", &LuaAPI::Random_Float);
	lua_register(State, "GetParent", &LuaAPI::GetParent);
	lua_register(State, "GetMemoryUsage", &LuaAPI::GetMemoryUsage);

	lua_pushnumber(State, PI);
	lua_setglobal(State, "PI");
}

//////////////////////////////////////////////////////////////////////////
// CLuaFunctionCall

CLuaFunctionCall::CLuaFunctionCall(const string &AFunctionName, int AResultsCount /*= 0*/) : FunctionName(AFunctionName), ArgumentsCount(0),
	ResultsCount(AResultsCount), Called(false), Broken(false)
{
	State = CLuaVirtualMachine::Instance()->State;
	if (!State)
	{
		Broken = true;
		return;
	}

	OldStackTop = lua_gettop(State);
	lua_getglobal(State, FunctionName.c_str());
}

CLuaFunctionCall::CLuaFunctionCall(const string &AObjectName, const string &AFunctionName, int AResultsCount /*= 0*/) : ObjectName(AObjectName),
	FunctionName(AFunctionName), ResultsCount(AResultsCount), ArgumentsCount(1), Called(false), Broken(false)
{
	State = CLuaVirtualMachine::Instance()->State;
	if (!State)
	{
		Broken = true;
		return;
	}

	OldStackTop = lua_gettop(State);

	lua_getglobal(State, ObjectName.c_str());
	if (lua_isnil(State, -1))
	{
		Log("ERROR", "An error occured while trying to call method function '%s:%s': no such object", ObjectName.c_str(), FunctionName.c_str());
		lua_pop(State, 1);
		Broken = true;
		return;
	}

	lua_getfield(State, -1, FunctionName.c_str());
	if (lua_isnil(State, -1))
	{
		Log("ERROR", "An error occured while trying to call method function '%s:%s': no such function", ObjectName.c_str(), FunctionName.c_str());
		lua_pop(State, 2);
		Broken = true;
		return;
	}

	lua_insert(State, -2);
}

CLuaFunctionCall::~CLuaFunctionCall()
{
	if (!Broken)
		lua_settop(State, OldStackTop);
}

bool CLuaFunctionCall::Call()
{
	if (Broken)
		return false;
	if (Called)
	{
		Log("ERROR", "Can't call the same function again, create new CLuaFunctionCall object");
		return false;
	}

	Called = true;

	if (lua_pcall(State, ArgumentsCount, ResultsCount, 0) != 0)
	{
		CLuaVirtualMachine::Instance()->LastError = lua_tostring(State, -1);
		Log("ERROR", "Lua: %s", CLuaVirtualMachine::Instance()->LastError.c_str());
		return false;
	}

	return true;
}

void CLuaFunctionCall::PushArgument(lua_Number Argument)
{
	if (Broken)
		return;
	if (Called)
	{
		Log("ERROR", "Can't push arguments after function call");
		return;
	}

	lua_pushnumber(State, Argument);

	ArgumentsCount++;
}

void CLuaFunctionCall::PushArgument(const string &Argument)
{
	if (Broken)
		return;
	if (Called)
	{
		Log("ERROR", "Can't push arguments after function call");
		return;
	}

	lua_pushstring(State, Argument.c_str());

	ArgumentsCount++;
}

void CLuaFunctionCall::PushArgument(void *Argument)
{
	if (Broken)
		return;
	if (Called)
	{
		Log("ERROR", "Can't push arguments after function call");
		return;
	}

	lua_pushlightuserdata(State, Argument);

	ArgumentsCount++;
}

//////////////////////////////////////////////////////////////////////////
// CScriptableComponent

CScriptableComponent::CScriptableComponent(CScript *AScript)
{
	CEventManager::Instance()->Subscribe("Create", this);
	CLuaVirtualMachine::Instance()->RunScript(AScript);
}

void CScriptableComponent::ProcessEvent(const CEvent &AEvent)
{
	if (AEvent.GetName() == "Create")
	{
		if (AEvent.GetData<string>("Name") == Name)
		{
			CLuaVirtualMachine::Instance()->CreateLuaObject(Name, this);
			CLuaVirtualMachine::Instance()->CallMethodFunction(Name, "OnCreate");
		}
	}
	else
	{
		CLuaFunctionCall fc(Name, "On" + AEvent.GetName());
		fc.PushArgument(const_cast<CEvent *>(&AEvent));
		fc.Call();
	}
}

