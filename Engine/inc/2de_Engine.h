#ifndef _2DE_ENGINE_H_
#define _2DE_ENGINE_H_

#include "2de_Core.h"
#include "2de_Config.h"
#include "2de_Event.h"
#include "2de_Factory.h"
#include "2de_GameUtils.h"
#include "2de_GraphicsLow.h"
#include "2de_GraphicsHigh.h"
#include "2de_Gui.h"
#include "2de_ImageUtils.h"
#include "2de_LuaUtils.h"
#include "2de_Math.h"
#include "2de_Resource.h"
#include "2de_Sound.h"
#include "2de_SpatialManager.h"
#include "2de_Update.h"
#include "2de_Xml.h"

const int ENGINE_VERSION = 0x001;	// it's a pity that it's not used.. i DO WANT some build counter, that increments automatically every build :)
const float FIXED_DELTA_TIME = 0.02f;
const int MAX_EVENT_FUNCTIONS = 8;
const int MAX_KEY_INPUT_FUNCTIONS = 8;

class CAbstractStateHandler
{
public:
	virtual void OnBeforeInitialize() { }
	virtual bool OnInitialize() { return true; }
	virtual void OnBeforeFinalize() { }
	virtual void OnFinalize() { }

	virtual void OnFocusGain() { }
	virtual void OnFocusLost() { }

	virtual bool OnArgumentsProcessing() { return true; }
};

class CEngine
{
public:
// IMMA BREAKING THE INCAPSULATION FOR DEBUGGING AND TESTING
	CAbstractSpatialManager *SpatialManager;
// SORRY
	static CEngine* Instance();

	bool Run(int argc, char *argv[]);
	void Pause();
	void ShutDown();

	bool isFinalizing() const;

	// Toggle - это переключить, а тут по смыслу - Set.. и геттеров нема - неудобно...
	void ToggleExitOnEscape(bool AdoExitOnEscape); // it seems to me, that it's too much overkill for such small option...  И как ты предлагаешь её выставлять, блеа? Ящитаю всё ок.
	void ToggleLimitFPS(bool AdoLimitFPS);
	void ToggleCalcFPS(bool AdoCalcFPS);
	void ToggleKeyRepeat(bool AdoKeyRepeat); // temporarily here.. basically, we need key repeat in GUI only, so in future it'll be handled by focusable GUI widget-groups (forms, panels, whatever..)
	void ToggleShowFPS(bool AdoShowFPS);

	template<typename T>
	void SetStateHandler();

	float GetDeltaTime() const;

	string GetProgramName() const;
	void SetProgramName(const string &AProgramName);

	unsigned long GetFPSLimit() const;
	void SetFPSLimit(unsigned long AFPSLimit);

	unsigned long GetFPS() const;


	// may leave it in something like CLowLevelInput, even when Event system will be ready...
	int keys[SDLK_LAST];	//FFUUU~ for sure. Wait till the Event system.
	Vector2 MousePos;

protected:
	CEngine();
	~CEngine();

private:
	CMouseCursor *Cursor;

	bool Initialize();
	void Finalize();
	bool ProcessEvents();
	bool ProcessArguments(int argc, char *argv[]);
	
	// possibly incapsulate into something.. too many things in this class are dedicated to fps...
	void CalcFPS();
	bool LimitFPS();
	
	bool Initialized;
	bool Finalizing;
	float dt;
	bool isHaveFocus;
	bool userReInit;

	bool doExitOnEscape;
	bool doLoadDefaultResourceList;
	bool doLimitFPS;
	bool doCalcFPS;
	unsigned long FPSCount;
	unsigned long FPSLimit;

	string ProgramName;

	//int EventFuncCount;
	//int KeyInputFuncCount;
	//CObject *KeyFuncCallers[MAX_KEY_INPUT_FUNCTIONS];
	CText *FPSText;
	

	// Временно здесь, будет заменено на систему KeyBinding'a и подписчиков.
	//EventFunc EventFunctions[MAX_EVENT_FUNCTIONS];
	//KeyInputFunc KeyInputFunctions[MAX_KEY_INPUT_FUNCTIONS];

	CAbstractStateHandler *StateHandler;

	static CEngine EngineInstance;
};

template<typename T>
void CEngine::SetStateHandler()
{
	if (StateHandler)
		delete StateHandler;
	
	StateHandler = new T;
}

#endif // _2DE_ENGINE_H_
