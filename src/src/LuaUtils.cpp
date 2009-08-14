#include "LuaUtils.h"
#include "Engine.h"
#include "OpenglUtils.h"

namespace
{
	// маленький помощник, чтобы самим не считать количество lua_push...() и lua_pop()
	class LuaStackGuard
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
	};

	int fun1(lua_State* L)
	{
		globalLuaState->innerFunction1();

		return 0;
	}

	int fun2(lua_State* L)
	{
		// LuaStackGuard здесь не нужен!
		// мы »«ћ≈Ќя≈ћ Lua-state!
		int retVal = globalLuaState->innerFunction2();

		lua_pushnumber(L, retVal);

		return 1;
	}

	int fun3(lua_State* L)
	{
		// LuaStackGuard здесь не нужен!
		// мы »«ћ≈Ќя≈ћ Lua-state!
		string retVal = globalLuaState->innerFunction3();

		lua_pushstring(L, retVal.c_str());

		return 1;
	}
}

GlobalLuaState::GlobalLuaState(const string& filename)
{
	L_ = luaL_newstate();
	luaL_openlibs(L_);

	// поскольку внутренние функции могут быть вызваны в процессе загрузки скрипта, 
	// то сначала регистрируем их
	lua_register(L_, "innerFunction1", fun1);
	lua_register(L_, "innerFunction2", fun2);
	lua_register(L_, "innerFunction3", fun3);

	// загружаем скрипт
	if(luaL_dofile(L_, filename.c_str()))
	{
		string err(lua_tostring(L_, -1));

		lua_close(L_);

		throw Exception(err);
	}
}

GlobalLuaState::~GlobalLuaState()
{
	lua_close(L_);
}

void GlobalLuaState::outerFunction1()
{
	// stack:
	LuaStackGuard stackGuard(L_);

	lua_getglobal(L_, "outerFunction1"); // stack: outerFunction1

	if(lua_pcall(L_, 0, 0, 0)) // stack:
	{
		string err(lua_tostring(L_, -1));

		lua_close(L_);

		throw Exception(err);
	}

	printf("GlobalLuaState::outerFunction1() called!\n");
}

int GlobalLuaState::outerFunction2()
{
	// stack:
	LuaStackGuard stackGuard(L_);

	lua_getglobal(L_, "outerFunction2"); // stack: outerFunction1

	if(lua_pcall(L_, 0, 1, 0)) // stack: число
	{
		string err(lua_tostring(L_, -1));

		lua_close(L_);

		throw Exception(err);
	}

	const int result = (int)lua_tonumber(L_, 1);

	printf("GlobalLuaState::outerFunction2() called! Result %d\n", result);

	return result;

	// после выхода из функции стек Lua должен быть в том же состо€нии, 
	// что и до входа в функцию.
	// stackGuard сам все подчистит за нами
}

std::string GlobalLuaState::outerFunction3()
{
	// stack:
	LuaStackGuard stackGuard(L_);

	lua_getglobal(L_, "outerFunction3"); // stack: outerFunction1

	if(lua_pcall(L_, 0, 1, 0)) // stack: строка
	{
		string err(lua_tostring(L_, -1));

		lua_close(L_);

		throw Exception(err);
	}

	string result;

	// функци€ может вернуть nil
	if(const char* s = lua_tostring(L_, 1))
	{
		result = s;
	}

	printf("GlobalLuaState::outerFunction3() called! Result %s\n", result.c_str());

	return result;

	// после выхода из функции стек Lua должен быть в том же состо€нии, 
	// что и до входа в функцию.
	// stackGuard сам все подчистит за нами
}

Vector2 pnts2[2];

void GlobalLuaState::innerFunction1()
{
	CEngine* Ninja = CEngine::Instance();
	CFactory *Factory = CFactory::Instance();

	CParticleSystem *ps;
	


	ps = dynamic_cast<CParticleSystem*>(Factory->Create(OBJ_PSYSTEM, NULL));
	ps->name = "psysaaaa";
	ps->Init();
	ps->info.sc = RGBAf(0.0f, 1.0f, 0.0f, 1.0f);
	ps->info.ec = RGBAf(0.0f, 0.0f, 1.0f, 0.5f);
	ps->visible = true;	
	pnts2[0] = Vector2(0, 480);
	pnts2[1] = Vector2(640, 480);
	ps->info.plife = 3;
	ps->info.emission = 500;///*debug 1 */ 3000;
	ps->info.startsize = 32;
	// debug //
	ps->info.sizevar = 8;
	ps->info.isSnow = true; // костыль
	ps->TexID = (dynamic_cast<CTexture*>(Ninja->TextureManager->GetObjectByName("Particle")))->GetTexID();
	ps->SetGeometry(pnts2, 2);

	int i;
	repeat(i, 201)
	{
		ps->Update(0.02f);
	}


	Factory->FreeInst();
	Ninja->FreeInst();

}

int GlobalLuaState::innerFunction2()
{
	printf("GlobalLuaState::innerFunction2() called!\n");

	return 12345;
}

string GlobalLuaState::innerFunction3()
{
	printf("GlobalLuaState::innerFunction3() called!\n");

	return "Hello from GlobalLuaState!";
}