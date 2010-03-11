#ifndef _2DE_ENGINE_H_
#define _2DE_ENGINE_H_

#include "2de_Core.h"

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#ifdef _WIN32
	#include <tchar.h>
#endif // _WIN32

#include "2de_GraphicsLow.h"
#include "2de_GraphicsHigh.h"
#include "2de_Xml.h"
#include "2de_Resource.h"
#include "2de_Sound.h"

const float FIXED_DELTA_TIME		=	0.02f;
const int ENGINE_VERSION			=	0x001;
const int MAX_EVENT_FUNCTIONS		=	8;
const int MAX_KEY_INPUT_FUNCTIONS	=	8;

class CEngine : public CTSingleton<CEngine>
{
public:
	enum EState
	{
		STATE_SCREEN_WIDTH,
		STATE_SCREEN_HEIGHT,
		STATE_WINDOW_CAPTION,
		STATE_DO_LIMIT_FPS,
		STATE_DO_CALC_FPS,
		STATE_FPS_LIMIT,
		STATE_FPS_COUNT,
		STATE_CONFIG_NAME,
		STATE_MOUSE_X,
		STATE_MOUSE_Y,
		STATE_MOUSE_XY,
		STATE_HIDE_CONSOLE_WINDOW,
		STATE_DELTA_TIME,
		STATE_CONFIG_PATH,
		STATE_USER_INIT_FUNC,
		STATE_UPDATE_FUNC,
		STATE_RENDER_FUNC,
		STATE_GUI_KEY_DOWN,
		STATE_GUI_KEY_UP,
		STATE_GUI_MOUSE_DOWN,
		STATE_GUI_MOUSE_UP,
		STATE_GUI_MOUSE_MOVE,
		STATE_GL_BG_COLOR,
	};

	int							keys[SDLK_LAST];	//FFUUU~ for sure. Wait til the Event system.
	Vector2						MousePos;

	void						SetState(EState state, void* value);	// "void*" ??? FFFUUUUU~
	void						GetState(EState state, void* value);	// Same.
	bool						AddEventFunction(EventFunc func);		// Until event system created.
	bool						AddKeyInputFunction(KeyInputFunc AKeyInputFunction, CObject* AKeyFuncCaller);
	int							CfgGetInt(char* ParamName);				// I think this one shouldn't be a member of CEnine. User knows the config name and path. But to do such from script. Should think more about it.
	bool						Run();	// If we have Run() we should have Pause() and ShutDown() or smthng.
	//bool						Pause();
	//bool						ShutDown();

	bool						Suicide();  // Временно в паблике
	string						ConfigFileName;
	string						ConfigFilePath;		//Temporary, until CConfig created. // Or no. We are not have CEngine::Config now.
	
private:
	bool						doLimitFps;
	bool						doLoadDefaultResourceList;
	unsigned long				FpsCount;
	unsigned long				FpsLimit;
	float						dt;
	bool						isHaveFocus;
	bool						userReInit;
	bool						Initialized;
	bool						doCalcFps;
	int							EventFuncCount;
	int							KeyInputFuncCount;
	CObject*					KeyFuncCallers[MAX_KEY_INPUT_FUNCTIONS];
	CGLWindow					*window;	// ORLY We need it here? // For sure we are not.
	
	bool						ClearLists();
	bool						Init();
	bool						MidInit();
	void						CalcFps();
	bool						LimitFps();
	
	bool						ProcessEvents();

	// Временно здесь, будет заменено на систему KeyBinding'a и подписчиков.
	EventFunc					EventFunctions[MAX_EVENT_FUNCTIONS];
	KeyInputFunc				KeyInputFunctions[MAX_KEY_INPUT_FUNCTIONS];
	bool						(*procUserInit)();			// ok
	bool						(*procUserSuicide)();		// wrong design
	bool						(*procFocusLostFunc)();		// ok
	bool						(*procFocusGainFunc)();		// ok
	bool						(*procUpdateFunc)(scalar);	// ok, yeah
	bool						(*procRenderFunc)();		// ok  NO wrong design; same for update and Init and so on. OOP MOTHERFUCKERS DO YOU USE IT!?
protected:
	friend class CTSingleton<CEngine>;
	CEngine();
	~CEngine();
};

#endif // _2DE_ENGINE_H_