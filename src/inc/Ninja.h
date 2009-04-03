#ifndef NINJA_H
#define NINJA_H

#pragma message("Compiling Ninja.h")

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

#define FIXED_DELTA_TIME		0.02f
#define NINJA_VERSION			0x100
#define MAX_EVENT_FUNCTIONS		10

#define STATE_SCREEN_WIDTH		0x01
#define STATE_DO_CALCFPS		0x02
#define STATE_SCREEN_HEIGHT		0x03
#define STATE_WINDOW_CAPTION	0x04
#define STATE_DO_LIMIT_FPS		0x05
#define STATE_DO_CALC_FPS		0x06
#define STATE_FPS_LIMIT			0x07
#define STATE_CONFIG_NAME		0x08
#define STATE_MOUSE_X			0x09
#define STATE_MOUSE_Y			0x0A
#define STATE_MOUSE_XY			0x0B

#define STATE_USER_INIT			0x81
#define STATE_UPDATE_FUNC		0x82
#define STATE_RENDER_FUNC		0x83
#define STATE_GUI_KEY_DOWN		0x84
#define STATE_GUI_KEY_UP		0x85
#define STATE_GUI_MOUSE_DOWN	0x86
#define STATE_GUI_MOUSE_UP		0x87
#define STATE_GUI_MOUSE_MOVE	0x88

typedef bool (*Callback)();
typedef bool (*KeyFunc)(char, SDLKey);
typedef bool (*MouseFunc)(int, int, unsigned char);
typedef bool (*InputFunc)(SDL_MouseButtonEvent&, SDL_MouseMotionEvent&, SDL_KeyboardEvent&);
typedef bool (*EventFunc)(SDL_Event&);



class CNinja
{
public:
	CResourceManager			ResourceManager;
	CTextureManager				*TextureManager;
	CRenderManager				RenderManager;
	CFontManager				*FontManager;
	CUpdateManager				UpdateManager;
	XMLTable					Config;
	BOOL						keys[SDLK_LAST];
	Vector2						MousePos;


	CFactory					*Factory;
	static CNinja*				Instance();
	void						FreeInst();
	void						appFocusChange(bool x_isHaveFocus);
	unsigned int				GetFps();
	float						Getdt();
	int							GetWindowHeight();
	int							GetWindowWidth();
	void						SetState(int state, void* value);
	void						GetState(int state, void* value);
	bool						AddEventFunction(EventFunc func);
	int							CfgGetInt(char* ParamName);
	bool						Run();
	char						*ResourceListPath;
	
private:
	string						ConfigFileName;
	bool						doLimitFps;
	unsigned int				Fps;
	unsigned int				FpsLimit;
	float						dt;
	bool						isHaveFocus;
	bool						doCalcFps;
	int							EventFuncCount;
	CGLWindow					window;


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
	static CNinja* _instance;
	static int _refcount;

	CNinja();
	~CNinja();
};

#endif NINJA_H
