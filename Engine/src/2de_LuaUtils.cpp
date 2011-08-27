#include "2de_LuaUtils.h"

#include "2de_Engine.h"

namespace Deku2d
{
	/**
	* LuaAPI - namespace for functions that is called from Lua scripts. The engine itself doesn't use them.
	*/

	namespace LuaAPI
	{
		// some of these functions are really will be left in the API, others are just temporarily ones for testing..

		// userdata Create(string ClassName, string Name)
		int Create(lua_State *L)
		{
			if (!lua_isstring(L, -1) || !lua_isstring(L, -2))
				LuaVirtualMachine->TriggerError("incorrect arguments given to Create API call");
			CGameObject* GameObject = dynamic_cast<CGameObject*>(Factory->CreateByName(lua_tostring(L, -2), lua_tostring(L, -1)));
			if (GameObject)
				lua_getglobal(L, GameObject->GetName().c_str());
			else
				lua_pushnil(L);
			//lua_pushlightuserdata(L, );
			return 1;
		}

		// userdata GetObject(string Name)
		int GetObject(lua_State *L)
		{
			if (!lua_isstring(L, -1))
				LuaVirtualMachine->TriggerError("incorrect arguments given to GetObject API call");

			lua_pushlightuserdata(L, Factory->Get<CObject>(lua_tostring(L, -1)));
			return 1;
		}

		// void Destroy(userdata Object)
		int Destroy(lua_State *L)
		{
			CObject *obj = static_cast<CObject *>(lua_touserdata(L, -1));
			if (!obj)
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in Destroy API call");

			Factory->Destroy(obj);
			return 0;
		}

		int DestroySubtree(lua_State *L)
		{
			CGameObject *obj = static_cast<CGameObject *>(lua_touserdata(L, -1));
			if (!obj)
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in DestroySubtree API call");
			obj->SetDestroyedSubtree();
			return 0;
		}

		// userdata Clone(userdata GameObject)
		int Clone(lua_State *L)
		{
			CGameObject *obj = static_cast<CGameObject *>(lua_touserdata(L, -1));
			if (!obj)
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in Clone API call");

			lua_pushlightuserdata(L, obj->Clone());
			return 1;
		}

		// userdata CloneTree(userdata GameObject)
		int CloneTree(lua_State *L)
		{
			CGameObject *obj = static_cast<CGameObject *>(lua_touserdata(L, -1));
			if (!obj)
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in CloneTree API call");


			lua_pushlightuserdata(L, obj->CloneTree());
			return 1;
		}

		// string GetName(userdata Object)
		int GetName(lua_State *L)
		{
			CObject *obj = static_cast<CObject *>(lua_touserdata(L, -1));
			if (!obj)
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in GetName API call");

			lua_pushstring(L, obj->GetName().c_str());
			return 1;
		}

		// userdata GetParent(userdata Object)
		int GetParent(lua_State *L)
		{
			CGameObject *obj = static_cast<CGameObject *>(lua_touserdata(L, -1));
			if (!obj)
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in GetParent API call");

			lua_pushlightuserdata(L, obj->GetParent());
			return 1;
		}

		// Temporary solution for asquiring children
		// Userdata object, number - child index
		int GetChild(lua_State *L)
		{
			CGameObject *obj = static_cast<CGameObject *>(lua_touserdata(L, -2));
			if (!obj)
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in GetChild API call");

			if (!lua_isnumber(L, -1))
				LuaVirtualMachine->TriggerError("incorrect arguments given to GetChild API call");

			int ChildIndex = lua_tonumber(L, -1);
			if (ChildIndex >= obj->GetChildCount())
				LuaVirtualMachine->TriggerError("incorrect arguments given to GetChild API call");

			lua_pushlightuserdata(L, obj->GetChild(ChildIndex));
			return 1;
		}

		// userdata FindFirstOfClass(userdata Object, string ClassName, bool ExceedPrototype)
		int FindFirstOfClass(lua_State *L)
		{
			CGameObject *obj = static_cast<CGameObject *>(lua_touserdata(L, -3));
			if (!obj)
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in FindFirstOfClass API call");
		
			if (!lua_isstring(L, -2))
				LuaVirtualMachine->TriggerError("incorrect arguments given to FindFirstOfClass API call");

			lua_pushlightuserdata(L, obj->FindFirstOfClass(lua_tostring(L, -2), lua_toboolean(L, -1)));
			return 1;
		}

		// void Attach(userdata Destination, userdata Object)
		int Attach(lua_State *L)
		{
			CGameObject *GameObjectSource = NULL, *GameObjectDestination = NULL;
			GameObjectDestination = static_cast<CGameObject *>(lua_touserdata(L, -2));
			if (!GameObjectDestination)
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in Attach API call");

			GameObjectSource = static_cast<CGameObject *>(lua_touserdata(L, -1));
			if (!GameObjectSource)
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in Attach API call");

			GameObjectDestination->Attach(GameObjectSource);
			return 0;
		}

		// void Detach(userdata Destination, userdata Object)
		int Detach(lua_State *L)
		{
			CGameObject *GameObjectSource = NULL, *GameObjectDestination = NULL;
			GameObjectDestination = static_cast<CGameObject *>(lua_touserdata(L, -2));
			if (!GameObjectDestination)
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in Detach API call");

			GameObjectSource = static_cast<CGameObject *>(lua_touserdata(L, -1));
			if (!GameObjectSource)
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in Detach API call");

			GameObjectDestination->Detach(GameObjectSource);
			return 0;
		}

		// void Log(string Event, string Text)
		int WriteToLog(lua_State *L)
		{
			if (!lua_isstring(L, -1) || !lua_isstring(L, -2))
				LuaVirtualMachine->TriggerError("incorrect arguments given to Log API call");

			Log(lua_tostring(L, -2), lua_tostring(L, -1));
			return 0;
		}

		// number GetDeltaTime()
		int GetDeltaTime(lua_State *L)	// This is case when just "dt" is fuckingly (spell checker suggests me to replace "fuckingly" with "motherfucking") ok.
		{
			lua_pushnumber(L, CEngine::Instance()->GetDeltaTime());
			return 1;
		}

		// string GetText(userdata TextObject)
		int GetText(lua_State *L)
		{
			CText *text = static_cast< CText *>(lua_touserdata(L, -1));
			if (!CheckType(text))
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in GetText API call");

			lua_pushstring(L, text->GetText().c_str());
			return 1;
		}

		// void SetText(userdata TextObject, string NewText)
		// Why not SetTextText if we already have SetTextFont on the other hand why not just SetFont?
		int SetText(lua_State *L)
		{
			CText *text = static_cast<CText *>(lua_touserdata(L, -2));
			if (!CheckType(text))
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in SetText API call");

			if (!lua_isstring(L, -1))
				LuaVirtualMachine->TriggerError("incorrect arguments given to SetText API call");

			text->SetText(lua_tostring(L, -1));

			return 0;
		}

		// void SetTextFont(userdata TextObject, userdata FontObject)
		int SetTextFont(lua_State *L)
		{
			CText *text = static_cast<CText *>(lua_touserdata(L, -2));
			if (!CheckType(text))
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in SetTextFont API call");
		
			CFont *font = static_cast<CFont *>(lua_touserdata(L, -1));
			if (!CheckType(font))
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in SetTextFont API call");

			text->SetFont(font);
			return 0;
		}

		// (number, number) GetPosition(userdata PlaceableComponent)
		int GetPosition(lua_State *L)
		{
			CPlaceableComponent *obj = static_cast<CPlaceableComponent *>(lua_touserdata(L, -1));
			if (!CheckType(obj))
			{
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in GetPosition API call");
			}

			lua_pushnumber(L, obj->GetPosition().x);
			lua_pushnumber(L, obj->GetPosition().y);
			return 2;
		}

		// void SetPosition(userdata PlaceableComponent, number X, number Y)
		int SetPosition(lua_State *L)
		{
			if (!lua_isnumber(L, -1) || !lua_isnumber(L, -2))
				LuaVirtualMachine->TriggerError("incorrect arguments given to SetPosition API call");

			CPlaceableComponent *obj = static_cast<CPlaceableComponent *>(lua_touserdata(L, -3));
			if (!CheckType(obj))
			{
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in SetPosition API call");
				return 0;
			}

			obj->SetPosition(Vector2(lua_tonumber(L, -2), lua_tonumber(L, -1)));
			return 0;
		}

		// table(CBox) GetBox(userdata PlaceableComponent)
		int GetBox(lua_State *L)
		{
			CPlaceableComponent *obj = static_cast<CPlaceableComponent *>(lua_touserdata(L, -1));
			if (!CheckType(obj))
			{
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in GetBox API call");
			}

			CLuaFunctionCall fc3("CBox", 1);

			CBox box = obj->GetBox();
			CLuaFunctionCall fc1("Vector2", 1);
			fc1.PushArgument(box.Min.x);
			fc1.PushArgument(box.Min.y);
			fc1.Call();

			CLuaFunctionCall fc2("Vector2", 1);
			fc2.PushArgument(box.Max.x);
			fc2.PushArgument(box.Max.y);
			fc2.Call();

			fc3.SetArgumentsCount(2);
			fc3.Call();

			return 1;
		}

		// number GetAngle(userdata PlaceableComponent)
		int GetAngle(lua_State *L)
		{
			CPlaceableComponent *obj = static_cast<CPlaceableComponent *>(lua_touserdata(L, -1));
			if (!CheckType(obj))
			{
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in GetAngle API call");
			}

			lua_pushnumber(L, obj->GetAngle());
			return 1;
		}

		// void SetAngle(userdata PlaceableComponent, number Angle)
		int SetAngle(lua_State *L)
		{
			if (!lua_isnumber(L, -1))
				LuaVirtualMachine->TriggerError("incorrect arguments given to SetAngle API call");

			CPlaceableComponent *obj = static_cast<CPlaceableComponent *>(lua_touserdata(L, -2));
			if (!CheckType(obj))
			{
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in SetAngle API call");
				return 0;
			}

			obj->SetAngle(lua_tonumber(L, -1));
			return 0;
		}

		// void SetColor(userdata RenderableComponent, number R, number G, number B, number A)
		int SetColor(lua_State *L)
		{
			if (!lua_isnumber(L, -1) || !lua_isnumber(L, -2) || !lua_isnumber(L, -3) || !lua_isnumber(L, -4))
				LuaVirtualMachine->TriggerError("incorrect arguments given to SetColor API call");

			CObject *obj = static_cast<CObject *>(lua_touserdata(L, -5));
			CRenderableComponent *rcobj = dynamic_cast<CRenderableComponent *>(obj);
			if (rcobj == NULL)
			{
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in SetColor API call");
				return 0;
			}

			rcobj->SetColor(RGBAf(lua_tonumber(L, -4), lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1)));
			return 0;
		}

		// void SetScript(userdata ScriptableComponent, userdata Script)
		int SetScript(lua_State *L)
		{
			CGameObject *ScriptableComponent = static_cast<CGameObject *>(lua_touserdata(L, -2));
			if (!CheckType(ScriptableComponent))
			{
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in SetScript API call");
				return 0;
			}

			CScript *Script = static_cast<CScript *>(lua_touserdata(L, -1));
			if (!CheckType(Script))
			{
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in SetScript API call");
				return 0;
			}

			ScriptableComponent->SetScript(Script);
			return 0;
		}

		// number GetWindowWidth()
		int GetWindowWidth(lua_State *L)
		{
			lua_pushnumber(L, GLWindow->GetWidth());
			return 1;
		}

		// number GetWindowHeight()
		int GetWindowHeight(lua_State *L)
		{
			lua_pushnumber(L, GLWindow->GetHeight());
			return 1;
		}

		// (number, number) GetWindowDimensions()
		int GetWindowDimensions(lua_State *L)
		{
			lua_pushnumber(L, GLWindow->GetWidth());
			lua_pushnumber(L, GLWindow->GetHeight());
			return 2;
		}

		// void SetClassName(userdata Object, string ClassName)
		int SetClassName(lua_State *L)
		{
			if (!lua_isstring(L, -1))
				LuaVirtualMachine->TriggerError("incorrect arguments given to SetClassName API call");

			CGameObject *obj = static_cast<CGameObject *>(lua_touserdata(L, -2));
			if (!obj)
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in SetClassName API call");

			obj->SetClassName(lua_tostring(L, -1));

			return 0;
		}

		// void SetInterval(userdata Timer, number Interval)
		int SetInterval(lua_State *L)
		{
			if (!lua_isnumber(L, -1))
				LuaVirtualMachine->TriggerError("incorrect arguments given to SetInterval API call");

			CTimerComponent *obj = static_cast<CTimerComponent *>(lua_touserdata(L, -2));
			if (!CheckType(obj))
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in SetInterval API call");

			obj->SetInterval(lua_tonumber(L, -1));
		}

		// void SetTimerEnabled(userdata Timer, bool Enabled)
		int SetTimerEnabled(lua_State *L)
		{
			if (!lua_isboolean(L, -1))
				LuaVirtualMachine->TriggerError("incorrect arguments given to SetTimerEnabled API call");

			CTimerComponent *obj = static_cast<CTimerComponent *>(lua_touserdata(L, -2));
			if (!CheckType(obj))
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in SetTimerEnabled API call");

			obj->SetEnabled(lua_toboolean(L, -1));
		}

		// void SubscribeToEvent(userdata Object, string EventName)
		int SubscribeToEvent(lua_State *L)
		{
			if (!lua_isstring(L, -1))
				LuaVirtualMachine->TriggerError("incorrect arguments given to SubscribeToEvent API call");

			CObject *obj = static_cast<CObject *>(lua_touserdata(L, -2));
			if (!obj)
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in SubscribeToEvent API call");

			EventManager->Subscribe(lua_tostring(L, -1), obj);

			return 0;
		}

		// void UnsubscribeFromEvent(string EventName, userdata Object)
		int UnsubscribeFromEvent(lua_State *L)
		{
			if (!lua_isstring(L, -2))
				LuaVirtualMachine->TriggerError("incorrect arguments given to UnsubscribeFromEvent API call");

			CObject *obj = static_cast<CObject *>(lua_touserdata(L, -1));
			if (!obj)
				LuaVirtualMachine->TriggerError("incorrect usage of light user data in UnsubscribeFromEvent API call");

			EventManager->Unsubscribe(lua_tostring(L, -2), obj);

			return 0;
		}

		// void TriggerEvent(string EventName, userdata Sender, table EventData = nil)
		int TriggerEvent(lua_State *L)
		{
			bool gotEventData = lua_istable(L, -1);

			if (!lua_isstring(L, -2 - gotEventData))
				LuaVirtualMachine->TriggerError("incorrect arguments given to TriggerEvent API call");

			CObject *obj = static_cast<CObject *>(lua_touserdata(L, -1 - gotEventData));

			CEvent *e = new CEvent(lua_tostring(L, -2 - gotEventData), obj);
			if (gotEventData)
			{
				lua_pushnil(L);
				while (lua_next(L, -2) != 0)
				{
					e->SetData(lua_tostring(L, -2), lua_tostring(L, -1));
					lua_pop(L, 1);
				}

			}
			EventManager->TriggerEvent(e);

			return 0;
		}

		// number sin(number n)
		int sin(lua_State *L)	// WHAT?! Lua cannot into sin() or what? // having multiple implementations (C++ and Lua) of the same functions can generally lead to some troubles.. it's VERY unlikely, though, but who knows?..
		{
			if (!lua_isnumber(L, -1))
				LuaVirtualMachine->TriggerError("incorrect arguments given to sin API call");

			lua_pushnumber(L, ::sin(lua_tonumber(L, -1)));
			return 1;
		}

		// number cos(number n)	// same as above
		int cos(lua_State *L)
		{
			if (!lua_isnumber(L, -1))
				LuaVirtualMachine->TriggerError("incorrect arguments given to cos API call");

			lua_pushnumber(L, ::cos(lua_tonumber(L, -1)));
			return 1;
		}

		// number Abs(number n)	-- господа, вы ебанулись в API такое экспортить. Пиздец. Ещё можно унарный минус заэкспортить. Так на всякий случай.
		int Abs(lua_State *L)	// same as above
		{
			if (!lua_isnumber(L, -1))
				LuaVirtualMachine->TriggerError("incorrect arguments given to Abs API call");

			lua_pushnumber(L, ::std::fabs(lua_tonumber(L, -1)));
			return 1;
		}
	#undef Random_Int
		// number Random_Int(number min, number max)
		int Random_Int(lua_State *L)
		{
			if (!lua_isnumber(L, -1) || !lua_isnumber(L, -2))
				LuaVirtualMachine->TriggerError("incorrect arguments given to Random_Int API call");

			lua_pushnumber(L, RandomRange<int>(lua_tonumber(L, -2), lua_tonumber(L, -1)));
			return 1;
		}
	#define Random_Int RandomRange<int>
	#undef Random_Float
		// number Random_Float(number min, number max)
		int Random_Float(lua_State *L)
		{
			if (!lua_isnumber(L, -1) || !lua_isnumber(L, -2))
				LuaVirtualMachine->TriggerError("incorrect arguments given to Random_Float API call");

			lua_pushnumber(L, RandomRange<float>(lua_tonumber(L, -2), lua_tonumber(L, -1)));
			return 1;
		}
	#define Random_Float RandomRange<float>
		// number GetMemoryUsage()
		int GetMemoryUsage(lua_State *L)
		{
			lua_pushnumber(L, LuaVirtualMachine->GetMemoryUsage());
			return 1;
		}

		/*void DebugPrintNode(CGameObject *GO)
		{
			static int nesting = 0;
			string out;

			for (int i = 0; i < nesting; i++)
				out += '\t';

			out += GO->GetName();

			Log("INFO", out.c_str());

			nesting++;
			for (vector<CGameObject *>::iterator it = GO->Children.begin(); it != GO->Children.end(); ++it)
			{
				DebugPrintNode(*it);
			}
			nesting--;
		}*/
		void DebugPrintNode(CGameObject *GO, map<int, bool> *LastChild)
		{
			static int nesting = 0;
			string out;

			for (int i = 0; i < nesting; i++)
			{
				if ((*LastChild)[i])
					out += "   ";
				else
					out += "|  ";
			}

			(*LastChild)[nesting + 1] = false;

			if ((*LastChild)[nesting])
				out += "`- ";
			else
				out += "|- ";

			out += GO->GetName();

			Log("INFO", out.c_str());

			CEvent *consoleOutputEvent = new CEvent("ConsoleOutput", NULL);
			consoleOutputEvent->SetData("Text", out.c_str());
			EventManager->TriggerEvent(consoleOutputEvent);

			nesting++;
			for (unsigned i = 0; i < GO->GetChildCount(); i++)
			{
				if (i == GO->GetChildCount() - 1)
					(*LastChild)[nesting] = true;
				DebugPrintNode(GO->GetChild(i), LastChild);
			}
			nesting--;
		}

		int DebugPrintComponentTree(lua_State *L)
		{
			map<int, bool> *LastChild = new map<int, bool>;
			(*LastChild)[0] = true;
			DebugPrintNode(CEngine::Instance()->RootGameObject, LastChild);
			delete LastChild;
			return 0;
		}

		int ConsoleOutput(lua_State *L)
		{
			if (!lua_isstring(L, -1))
				LuaVirtualMachine->TriggerError("incorrect arguments given to ConsoleOutput API call");

			CEvent *consoleOutputEvent = new CEvent("ConsoleOutput", NULL);
			consoleOutputEvent->SetData("Text", lua_tostring(L, -1));
			EventManager->TriggerEvent(consoleOutputEvent);

			return 0;
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
			if (!FileSystem::Exists(Filename))
			{
				Log("ERROR", "Can't load script from a file named '%s': file doesn't exist", Filename.c_str());
				return false;
			}
		}
		else if (Source == LOAD_SOURCE_MEMORY)
		{
			CMemory mem(static_cast<byte*>(MemoryLoadData), MemoryLoadLength, CStorage::OPEN_MODE_READ);
			ScriptText = mem.GetContent();
		}
		else
		{
			Log("ERROR", "Can't load script: no load source specified");
			return false;
		}

		CResource::BasicLoad();

		return true;
	}

	string CScript::GetScriptText() const
	{
		return ScriptText;
	}

	void CScript::Unload()
	{
		// Guess that SCript doesn't have memory-heavy part and do nothing here
		CResource::BasicUnload();
	}

	void CScript::SetRunned()
	{
		Runned = true;
	}

	bool CScript::IsRunned()
	{
		return Runned;
	}

	CScript::CScript() : Runned(false)
	{

	}

	//////////////////////////////////////////////////////////////////////////
	// CLuaVirtualMachine

	CLuaVirtualMachine::CLuaVirtualMachine()
	{
		SetName("CLuaVirtualMachine");
		
		L = luaL_newstate();
		luaL_openlibs(L);
		
		if (!L)
		{
			Log("ERROR", "Lua state initialization failed");
			return;
		}

		string scriptsPath = Environment::Paths::GetWorkingDirectory() + "/" + ResourceManager->GetDataPath() + "/Scripts/";
		lua_getglobal(L, "package");
		lua_getfield(L, -1, "path");
		lua_pushstring(L, (";" + scriptsPath + "?.lua").c_str() );
		lua_concat(L, 2);
		lua_setfield(L, -2, "path");
		lua_pop(L, 1);
		RegisterStandardAPI();
	}

	CLuaVirtualMachine::~CLuaVirtualMachine()
	{
		lua_close(L);
	}

	bool CLuaVirtualMachine::RunFile(const string &AFilename)
	{
		if (!L)
			return false;

		if (luaL_dofile(L, AFilename.c_str()) != 0)
		{
			OutputError(lua_tostring(L, -1));
			return false;
		}

		return true;
	}

	bool CLuaVirtualMachine::RunString(const string &AString)
	{
		if (!L)
			return false;
		
		if (luaL_dostring(L, AString.c_str()) != 0)
		{
			OutputError(lua_tostring(L, -1));
			return false;
		}

		return true;
	}

	bool CLuaVirtualMachine::RunScript(CScript *AScript)
	{
		if (!L)
			return false;
		if (AScript == NULL)
		{
			Log("ERROR", "Script is NULL in CLuaVirtualMachine::RunScript");
			return false;
		}

		if (AScript->IsRunned())
			return true;

		AScript->SetRunned();
		if (AScript->GetLoadSource() == CResource::LOAD_SOURCE_FILE)
		{
			return RunFile(AScript->GetFilename());
		}
		else if (AScript->GetLoadSource() == CResource::LOAD_SOURCE_MEMORY)
		{
			return RunString(AScript->GetScriptText());
		}
		
		return false;
	}

	void CLuaVirtualMachine::RegisterAPIFunction(const string &AName, lua_CFunction AFunc)
	{
		if (!L)
			return;

		lua_register(L, AName.c_str(), AFunc);
	}

	bool CLuaVirtualMachine::CallFunction(const string &AFunctionName)
	{
		return CLuaFunctionCall(AFunctionName).Call();
	}

	bool CLuaVirtualMachine::CallMethodFunction(const string &AObjectName, const string &AFunctionName)
	{
		return CLuaFunctionCall(AObjectName, AFunctionName).Call();
	}

	static void DeepCopy(lua_State *L)
	{
		if (!lua_istable(L, -1))
		{
			lua_pushvalue(L, -1);
			return;
		}
		int table_idx = lua_gettop(L);
		
		lua_getmetatable(L, -1);

		lua_newtable(L);
		int newtable_idx = lua_gettop(L);

		lua_pushnil(L);
		while (lua_next(L, table_idx) != 0)
		{
			lua_pushvalue(L, -2);
			lua_insert(L, -2);
			if (lua_istable(L, -1))
			{
				DeepCopy(L);
				lua_insert(L, -2);
				lua_pop(L, 1);
			}

			lua_settable(L, newtable_idx);
		}

		lua_insert(L, -2);
		lua_setmetatable(L, -1);
	}

	/**
	* CLuaVirtualMachine::CreateLuaObject - creates a new Lua object of AClassName class,
	* puts it in the global namespace table under AName name and sets its "object" field to AObject.
	*/

	void CLuaVirtualMachine::CreateLuaObject(const string &AClassName, const string &AName, CObject *AObject)
	{
		if (AClassName != AName)
		{
			lua_getglobal(L, AClassName.c_str());
			if (lua_isnil(L, -1) || !lua_istable(L, -1))
			{
				Log("ERROR", "An error occurred while creating '%s': its class '%s' must be defined in Lua script first", AName.c_str(), AClassName.c_str());
				lua_pop(L, 1);
				return;
			}

			DeepCopy(L);

			lua_setglobal(L, AName.c_str());

	//		lua_pop(L, 1);
		}

		lua_getglobal(L, AName.c_str());
		if (lua_isnil(L, -1))
		{
			Log("ERROR", "An error occurred while creating '%s': object must be defined in Lua script first", AName.c_str());
			lua_pop(L, 1);
			return;
		}
		lua_pushlightuserdata(L, AObject);
		lua_setfield(L, -2, "object");
		lua_pop(L, 1);

		if (IsMethodFunctionExists(AName, "OnCreate"))
			CallMethodFunction(AName, "OnCreate");

		Log("INFO", "Lua object CREATED: '%s' of class '%s'", AName.c_str(), AClassName.c_str());
	}

	void CLuaVirtualMachine::SetLocalNamesFields(CGameObject *AGameObject)
	{
		for (CGameObject::LNOMType::iterator i = AGameObject->LocalNameObjectMapping.begin(); 
			i != AGameObject->LocalNameObjectMapping.end(); ++i)
		{
			SetReferenceField(AGameObject, i->first, i->second);
		}
	}

	void CLuaVirtualMachine::SetReferenceField(CObject *AObject, const string &AFieldName, CObject *AReference)
	{
		lua_getglobal(L, AObject->GetName().c_str());
		if (lua_isnil(L, -1))
		{
			Log("ERROR", "An error occurred while setting reference field '%s' for object '%s': object Lua table doesn't exist",
				AFieldName.c_str(), AObject->GetName().c_str());
			lua_pop(L, 1);
			return;
		}

		if (AReference)
		{
			lua_getglobal(L, AReference->GetName().c_str());
			if (lua_isnil(L, -1))
			{
				lua_pop(L, 1);
				lua_pushlightuserdata(L, AReference);
			}
		}
		else
		{
			lua_pushnil(L);
		}

		lua_setfield(L, -2, AFieldName.c_str());
		lua_pop(L, 1);
	}

	void CLuaVirtualMachine::DestroyLuaObject(const CGameObject &AObject)
	{
		lua_getglobal(L, AObject.GetName().c_str());
		if (lua_isnil(L, -1))
		{
			lua_pop(L, 1);
			return;
		}
		lua_pop(L, 1);

		lua_pushnil(L);
		lua_setglobal(L, AObject.GetName().c_str());

		Log("INFO", "Lua object DESTROYED: '%s'", AObject.GetName().c_str());
	}

	void CLuaVirtualMachine::PushEventTable(const CEvent &AEvent)
	{
		lua_newtable(L);

		lua_pushlightuserdata(L, AEvent.GetSender());
		lua_setfield(L, -2, "sender");

		lua_pushstring(L, AEvent.GetName().c_str());
		lua_setfield(L, -2, "name");

		for (CEvent::DataIterator it = AEvent.Begin(); it != AEvent.End(); ++it)
		{
			lua_pushstring(L, it->second.c_str());
			lua_setfield(L, -2, it->first.c_str());
		}
	}

	bool CLuaVirtualMachine::IsObjectExists(const string &AObjectName)
	{
		lua_getglobal(L, AObjectName.c_str());

		bool result = !lua_isnil(L, -1) && lua_istable(L, -1);
		lua_pop(L, 1);
		return result;
	}

	bool CLuaVirtualMachine::IsMethodFunctionExists(const string &AObjectName, const string &AFunctionName)
	{
		lua_getglobal(L, AObjectName.c_str());
		if (lua_isnil(L, -1))
		{
			lua_pop(L, 1);
			return false;
		}

		lua_getfield(L, -1, AFunctionName.c_str());

		bool result = lua_isfunction(L, -1);
		lua_pop(L, 2);
		return result;
	}

	int CLuaVirtualMachine::GetMemoryUsage() const
	{
		if (!L)
			return 0;

		return lua_gc(L, LUA_GCCOUNT, 0);
	}

	void CLuaVirtualMachine::RunGC()
	{
		if (!L)
			return;

		lua_gc(L, LUA_GCCOLLECT, 0);
	}

	string CLuaVirtualMachine::GetLastError() const
	{
		return LastError;
	}

	void CLuaVirtualMachine::TriggerError(const string &AMessage, ...)
	{
		if (!L)
			return;
		
		va_list args;
		va_start(args, AMessage);
		luaL_error(L, AMessage.c_str(), args);
		va_end(args);
	}

	void CLuaVirtualMachine::RegisterStandardAPI()
	{
		// object management
		lua_register(L, "Create", &LuaAPI::Create);
		lua_register(L, "GetObject", &LuaAPI::GetObject);
		lua_register(L, "Destroy", &LuaAPI::Destroy);
		lua_register(L, "DestroySubtree", &LuaAPI::DestroySubtree);
		lua_register(L, "Clone", &LuaAPI::Clone);
		lua_register(L, "CloneTree", &LuaAPI::CloneTree);
		lua_register(L, "GetName", &LuaAPI::GetName);
		lua_register(L, "GetParent", &LuaAPI::GetParent);
		lua_register(L, "GetChild", &LuaAPI::GetChild);
		lua_register(L, "FindFirstOfClass", &LuaAPI::FindFirstOfClass);
		lua_register(L, "Attach", &LuaAPI::Attach);
		lua_register(L, "Detach", &LuaAPI::Detach);

		// engine
		lua_register(L, "Log", &LuaAPI::WriteToLog);
		lua_register(L, "GetDeltaTime", &LuaAPI::GetDeltaTime);

		// various getters and setters, uncategorized yet
		lua_register(L, "SetText", &LuaAPI::SetText);
		lua_register(L, "GetText", &LuaAPI::GetText);
		lua_register(L, "SetTextFont", &LuaAPI::SetTextFont);
		lua_register(L, "GetPosition", &LuaAPI::GetPosition);
		lua_register(L, "SetPosition", &LuaAPI::SetPosition);
		lua_register(L, "GetBox", &LuaAPI::GetBox);
		lua_register(L, "GetAngle", &LuaAPI::GetAngle);
		lua_register(L, "SetAngle", &LuaAPI::SetAngle);
		lua_register(L, "SetColor", &LuaAPI::SetColor);
		lua_register(L, "SetScript", &LuaAPI::SetScript);
		lua_register(L, "GetWindowWidth", &LuaAPI::GetWindowWidth);
		lua_register(L, "GetWindowHeight", &LuaAPI::GetWindowHeight);
		lua_register(L, "GetWindowDimensions", &LuaAPI::GetWindowDimensions);
		lua_register(L, "SetClassName", &LuaAPI::SetClassName);
		lua_register(L, "SetInterval", &LuaAPI::SetInterval);
		lua_register(L, "SetTimerEnabled", &LuaAPI::SetTimerEnabled);

		// event management
		lua_register(L, "SubscribeToEvent", &LuaAPI::SubscribeToEvent);
		lua_register(L, "UnsubscribeFromEvent", &LuaAPI::UnsubscribeFromEvent);
		lua_register(L, "TriggerEvent", &LuaAPI::TriggerEvent);

		// math
		lua_register(L, "sin", &LuaAPI::sin);
		lua_register(L, "cos", &LuaAPI::cos);
		lua_register(L, "Abs", &LuaAPI::Abs);
	#undef Random_Int
	#undef Random_Float
		lua_register(L, "Random_Int", &LuaAPI::Random_Int);
		lua_register(L, "Random_Float", &LuaAPI::Random_Float);
	#define Random_Int RandomRange<int>
	#define Random_Float RandomRange<float>
		// lua & debug
		lua_register(L, "GetMemoryUsage", &LuaAPI::GetMemoryUsage);
		lua_register(L, "DebugPrintComponentTree", &LuaAPI::DebugPrintComponentTree);
		lua_register(L, "ConsoleOutput", &LuaAPI::ConsoleOutput);

		// constants
		lua_pushnumber(L, Const::Math::PI);
		lua_setglobal(L, "PI");
	}

	void CLuaVirtualMachine::OutputError(const string &AError)
	{
		LastError = AError;
		Log("ERROR", "Lua: %s", AError.c_str());

		CEvent *consoleOutputEvent = new CEvent("ConsoleOutput", NULL);
		consoleOutputEvent->SetData("Text", AError);
		consoleOutputEvent->SetData("Type", "Error");
		EventManager->TriggerEvent(consoleOutputEvent);
	}

	//////////////////////////////////////////////////////////////////////////
	// CLuaFunctionCall

	CLuaFunctionCall::CLuaFunctionCall(const string &AFunctionName, int AResultsCount /*= 0*/) : FunctionName(AFunctionName), ArgumentsCount(0),
		ResultsCount(AResultsCount), Called(false), Broken(false), MethodCall(false)
	{
		L = LuaVirtualMachine->L;
		if (!L)
		{
			Broken = true;
			return;
		}

		OldStackTop = lua_gettop(L);
		lua_getglobal(L, FunctionName.c_str());
		if (!lua_isfunction(L, -1))
		{
			Log("ERROR", "An error occurred while trying to call function '%s': no such function", FunctionName.c_str());
			lua_pop(L, 1);
			Broken = true;
			return;
		}
	}

	CLuaFunctionCall::CLuaFunctionCall(const string &AObjectName, const string &AFunctionName, int AResultsCount /*= 0*/) : ObjectName(AObjectName),
		FunctionName(AFunctionName), ResultsCount(AResultsCount), ArgumentsCount(0), Called(false), Broken(false), MethodCall(true)
	{
		L = LuaVirtualMachine->L;
		if (!L)
		{
			Broken = true;
			return;
		}

		OldStackTop = lua_gettop(L);

		lua_getglobal(L, ObjectName.c_str());
		if (lua_isnil(L, -1))
		{
			Log("ERROR", "An error occurred while trying to call method function '%s:%s': no such object", ObjectName.c_str(), FunctionName.c_str());
			lua_pop(L, 1);
			Broken = true;
			return;
		}
		lua_getfield(L, -1, FunctionName.c_str());
		if (!lua_isfunction(L, -1))
		{
			Log("ERROR", "An error occurred while trying to call method function '%s:%s': no such function", ObjectName.c_str(), FunctionName.c_str());
			lua_pop(L, 2);
			Broken = true;
			return;
		}
		lua_insert(L, -2);
	}

	CLuaFunctionCall::~CLuaFunctionCall()
	{
		/*if (!Broken)
			lua_settop(State, OldStackTop);*/
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
		if (lua_pcall(L, MethodCall ? ArgumentsCount + 1 : ArgumentsCount, ResultsCount, 0) != 0)
		{
			LuaVirtualMachine->OutputError(lua_tostring(L, -1));
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

		lua_pushnumber(L, Argument);

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

		lua_pushstring(L, Argument.c_str());

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

		lua_pushlightuserdata(L, Argument);

		ArgumentsCount++;
	}

	void CLuaFunctionCall::SetArgumentsCount(int AArgumentsCount)
	{
		if (Called)
		{
			Log("ERROR", "Can't set arguments count after function call");
			return;
		}

		ArgumentsCount = AArgumentsCount;
	}

}	//	namespace Deku2d
