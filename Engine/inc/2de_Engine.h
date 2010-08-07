#ifndef _2DE_ENGINE_H_
#define _2DE_ENGINE_H_

#include "2de_Core.h"
#include "2de_GameUtils.h"
#include "2de_GraphicsLow.h"
#include "2de_GraphicsHigh.h"
#include "2de_Gui.h"
#include "2de_ImageUtils.h"
#include "2de_MathUtils.h"
#include "2de_Resource.h"
#include "2de_Sound.h"
#include "2de_Update.h"
#include "2de_Xml.h"

const float FIXED_DELTA_TIME		=	0.02f;
const int ENGINE_VERSION			=	0x001; // it's a pity that it's not used.. i DO WANT some build counter, that increments automatically every build :)
const int MAX_EVENT_FUNCTIONS		=	8;
const int MAX_KEY_INPUT_FUNCTIONS	=	8;

class CAbstractStateHandler
{
public:
	virtual void OnBeforeInitialize() { }
	virtual void OnInitialize() { }
	virtual void OnBeforeFinalize() { }
	virtual void OnFinalize() { }

	virtual void OnFocusGain() { }
	virtual void OnFocusLost() { }
};

class CEngine
{
public:
	enum EState // @todo: избавиться от этих стейтов и придумать более хороший способ делать то же самое.
	{		// начинаю избавляться... потихонечку буду убирать то, что успешно делается в другом месте..
		STATE_DO_LIMIT_FPS,	// <
		STATE_DO_CALC_FPS,	//	инкапуслировать во что-нибудь
		STATE_FPS_LIMIT,	//
		STATE_FPS_COUNT,	// >
		STATE_CONFIG_NAME,	//	инкапуслировать в CConfig
		STATE_DELTA_TIME,	//	сделать фанарный геттер
		STATE_UPDATE_FUNC,	// <	сделать по принципу CAbstractStateHandler.. не сделал сразу, потому что не смог придумать имя для класс.. CAbstractGlobalUpdateAndRenderHandler lol..
		STATE_RENDER_FUNC,	// >
		STATE_GL_BG_COLOR,	//	инкапсулировать в какую-нибудь графическую подсистему..
	};

	static CEngine* Instance();

	// deprecate as soon as possible!
	void SetState(EState state, void* value);	// "void*" ??? FFFUUUUU~
	void GetState(EState state, void* value);	// Same.

	bool AddEventFunction(EventFunc func);		// Until event system created.
	bool AddKeyInputFunction(KeyInputFunc AKeyInputFunction, CObject* AKeyFuncCaller);

	bool Run();
	void Pause();
	void ShutDown();

	void ToggleExitOnEscape(bool AdoExitOnEscape); // it seems to me, that it's too much overkill for such small option... 

	template<typename T>
	void SetStateHandler();

	// may leave it in something like CLowLevelInput, even when Event system will be ready...
	int keys[SDLK_LAST];	//FFUUU~ for sure. Wait till the Event system.
	Vector2 MousePos;

	string ConfigFileName;	// temporary, until CConfig created..
				// maybe standardize it, something like "Config.xml", or make it more wide concept, like program name - so ProgramName + ".xml"
protected:
	CEngine();
	~CEngine();

private:
	bool Initialize();
	void Finalize();
	bool ProcessEvents();
	
	// possibly incapsulate into something.. too many things in this class are dedicated to fps...
	void CalcFps();
	bool LimitFps();
	
	bool Initialized;
	float dt;
	bool isHaveFocus;
	bool userReInit;

	bool doExitOnEscape;
	bool doLoadDefaultResourceList;
	bool doLimitFps;
	bool doCalcFps;
	bool doShowFPS;
	unsigned long FpsCount;
	unsigned long FpsLimit;

	int EventFuncCount;
	int KeyInputFuncCount;
	CObject *KeyFuncCallers[MAX_KEY_INPUT_FUNCTIONS];
	CText *FPSText;
	

	// Временно здесь, будет заменено на систему KeyBinding'a и подписчиков.
	EventFunc EventFunctions[MAX_EVENT_FUNCTIONS];
	KeyInputFunc KeyInputFunctions[MAX_KEY_INPUT_FUNCTIONS];

	bool (*procUpdateFunc)(float);		// ok, yeah
	bool (*procRenderFunc)();		// ok  NO wrong design; 
			// same for update and Init and so on. OOP MOTHERFUCKERS DO YOU USE IT!?
			// 	yeah, now we use it.. Update and Render will use it too, soon.. as soon as I think out some sane name for "CAbstractGlobalUpdateAndRenderHandler" class, lol...

	CAbstractStateHandler *StateHandler;

	static CEngine MainEngineInstance;
};

template<typename T>
void CEngine::SetStateHandler()
{
	if (StateHandler)
		delete StateHandler;
	
	StateHandler = new T;
}

#endif // _2DE_ENGINE_H_
