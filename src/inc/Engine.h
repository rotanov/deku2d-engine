#ifndef NINJA_H
#define NINJA_H

#pragma message("Compiling Engine.h")

#include "CoreUtils.h"

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>
#include <SDL.h>

#include "OpenglUtils.h"
#include "Gui.h"
#include "Xml.h"
#include "resource.h"

// Константы

#define FIXED_DELTA_TIME			0.02f
#define ENGINE_VERSION				0x001
#define MAX_EVENT_FUNCTIONS			10

// Состояния переменных движка, для Set/Get State
// пока что тут неполный набор.

#define STATE_SCREEN_WIDTH			0x01
#define STATE_SCREEN_HEIGHT			0x03
#define STATE_WINDOW_CAPTION		0x04
#define STATE_DO_LIMIT_FPS			0x05
#define STATE_DO_CALC_FPS			0x06
#define STATE_FPS_LIMIT				0x07
#define STATE_FPS_COUNT				0x02
#define STATE_CONFIG_NAME			0x08
#define STATE_MOUSE_X				0x09
#define STATE_MOUSE_Y				0x0A
#define STATE_MOUSE_XY				0x0B
#define STATE_HIDE_CONSOLE_WINDOW	0x0C
#define STATE_DELTA_TIME			0x0D

#define STATE_USER_INIT_FUNC		0x81
#define STATE_UPDATE_FUNC			0x82
#define STATE_RENDER_FUNC			0x83
#define STATE_GUI_KEY_DOWN			0x84
#define STATE_GUI_KEY_UP			0x85
#define STATE_GUI_MOUSE_DOWN		0x86
#define STATE_GUI_MOUSE_UP			0x87
#define STATE_GUI_MOUSE_MOVE		0x88

#define STATE_GL_BG_COLOR			0xA0

typedef bool (*Callback)();
typedef bool (*KeyFunc)(char, SDLKey);
typedef bool (*MouseFunc)(int, int, unsigned char);
typedef bool (*InputFunc)(SDL_MouseButtonEvent&, SDL_MouseMotionEvent&, SDL_KeyboardEvent&);
typedef bool (*EventFunc)(SDL_Event&);



class CEngine
{
public:
	CFactory					*Factory; // Фабрика объектов. Синглтон.
	CResourceManager			ResourceManager; // А почему тут инастанс
	CTextureManager				*TextureManager; // ...а тут указатель?
	CRenderManager				RenderManager; // а тут инстанс!
	CFontManager				*FontManager; // ...а тут опять указатель?! 
	CUpdateManager				UpdateManager; // не, ну ёбаны в рот.
	XMLTable					Config; // Да! Это  - конфиг. Нахуй его хранить тут вот только мне непонятно... Ведь он нужен только при загрузке.
	BOOL						keys[SDLK_LAST];  // FFFFFFFFUUUUUUUUUU~ ?
	Vector2						MousePos;

	static CEngine*				Instance();
	void						FreeInst();
	void						SetState(int state, void* value);
	void						GetState(int state, void* value);
	bool						AddEventFunction(EventFunc func);
	int							CfgGetInt(char* ParamName);
	bool						Run();
	
private:
	char						*ResourceListPath;
	string						ConfigFileName;
	bool						doLimitFps;
	bool						doLoadDefaultResourceList;
	DWORD						FpsCount;
	DWORD						FpsLimit;
	float						dt;
	bool						isHaveFocus;
	bool						userReInit;
	bool						Initialized;
	bool						doCalcFps;
	int							EventFuncCount;
	CGLWindow					window;
	
	bool						ClearLists();
	bool						Init();
	bool						MidInit();
	void						CalcFps();
	bool						LimitFps();
	bool						Suicide();
	bool						ProcessEvents();

	EventFunc					EventFunctions[MAX_EVENT_FUNCTIONS];
	bool						(*procUserInit)();
	bool						(*procUserSuicide)();
	bool						(*procFocusLostFunc)();
	bool						(*procFocusGainFunc)();
	bool						(*procUpdateFunc)();
	bool						(*procRenderFunc)();
	bool						(*procGUIGetKeyDown)(char k, SDLKey sym);
	bool						(*procGUIGetKeyUp)(char k, SDLKey sym);
	bool						(*procGUIGetMouseDown)(int x, int y, unsigned char button);
	bool						(*procGUIGetMouseUp)(int x, int y, unsigned char button);
	bool						(*procGUIGetMouseMove)(int x, int y, unsigned char button);

protected:
	static CEngine* _instance;
	static int _refcount;

	CEngine();
	~CEngine();
};

#endif NINJA_H
