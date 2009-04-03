		#include "Ninja.h"

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif

//-------------------------------------------//
//				Ninja stuff					 //
//-------------------------------------------//

CNinja::CNinja()
{
	memset(keys, 0, sizeof(keys));
	doLimitFps = false; 
	isHaveFocus = true;
	doCalcFps = true;
	procUserInit = NULL;
	procUserSuicide = NULL;
	procFocusLostFunc = NULL;
	procFocusGainFunc = NULL;
	procUpdateFunc = NULL;
	procRenderFunc = NULL;
	procGUIGetKeyDown = NULL;
	procGUIGetKeyUp = NULL;
	procGUIGetMouseDown = NULL;
	procGUIGetMouseUp = NULL;
	procGUIGetMouseMove = NULL;
	Factory = CFactory::Instance();
	FontManager = CFontManager::Instance("ninja.cpp");
	_instance = NULL;
	_refcount = 0;
	ConfigFileName = CONFIG_FILE_NAME;
	ConfigFileName += "configuration.xml";
	EventFuncCount = 0;
}
CNinja::~CNinja(){}

CNinja* CNinja::Instance()
{
	if (_instance == NULL)
	{
		_instance = new CNinja;
	}
	_refcount++;
	return _instance;
}

void CNinja::FreeInst()
{
	_refcount--;
	if (!_refcount)
	{
		delete this;
		Log("INFO", "Ninja deleted from memory.");
		_instance = NULL;
	}
}

int	CNinja::GetWindowHeight()
{
	return window.height;
}
int CNinja::GetWindowWidth()
{
	return window.width;
}

unsigned int CNinja::GetFps()
{
	return Fps;
}

float CNinja::Getdt()
{
	return dt;
}

void CNinja::CalcFps()
{
	static DWORD DTime, _llt, lt, fr;
	
	DWORD ct = SDL_GetTicks();
	DTime = ct-_llt;
	dt = (float)DTime / 1000.0f;
	_llt = ct;
	if (ct-lt >= 1000)
	{
		Fps = ((fr)*1000)/(ct-lt);
		lt = ct;
		fr = 0;
	}		
	fr++;
}

bool CNinja::LimitFps()
{
	if (!doLimitFps)
		return true;
	static DWORD _lt = 0, _llt = 0;

	_lt = SDL_GetTicks();
	if (_lt - _llt >= FpsLimit)
	{				
		_llt = _lt;
		return true;
	}
	return false;
}

void CNinja::appFocusChange(bool x_isHaveFocus)
{
	isHaveFocus = x_isHaveFocus;
	if (isHaveFocus)
	{
		
	}
	else
	{

	}
}

void CNinja::SetState(int state, void* value)
	{
		switch(state)
		{
			case STATE_USER_INIT:
				procUserInit = (Callback) value;
				break;
			case STATE_UPDATE_FUNC:
				procUpdateFunc = (Callback) value;
				break;
			case STATE_RENDER_FUNC:
				procRenderFunc = (Callback) value;
				break;
			case STATE_DO_LIMIT_FPS:
				doLimitFps = !!value;
				break;
			case STATE_DO_CALC_FPS:
				doCalcFps = !!value;
				break;
			case STATE_FPS_LIMIT:
				FpsLimit = (unsigned int)value;
				break;
			case STATE_SCREEN_WIDTH:
				window.width = (int)value;
				break;
			case STATE_SCREEN_HEIGHT:
				window.height = (int)value;
				break;
			case STATE_WINDOW_CAPTION:
				window.caption = (char*)value;
				break;
			case STATE_GUI_KEY_DOWN:
				procGUIGetKeyDown = (KeyFunc)value;
				break;
			case STATE_GUI_MOUSE_DOWN:
				procGUIGetMouseDown = (MouseFunc)value;
				break;
			case STATE_GUI_KEY_UP:
				procGUIGetKeyUp = (KeyFunc)value;
				break;
			case STATE_GUI_MOUSE_UP:
				procGUIGetMouseUp = (MouseFunc)value;
				break;
			case STATE_GUI_MOUSE_MOVE:
				procGUIGetMouseMove = (MouseFunc)value;
				break;
			case STATE_CONFIG_NAME:
				{
					ConfigFileName = CONFIG_FILE_NAME;
					ConfigFileName += (char*)value;
				}
		}
	}

bool CNinja::Init()
{
	#ifdef WIN32
		#define WIN32_LEAN_AND_MEAN		
		HMODULE hmodule = GetModuleHandle("Ninja Engine.exe");
 		char * pathexe = new char[1024];
		GetModuleFileName(hmodule, pathexe, 256);
		HWND hwnd = FindWindow("ConsoleWindowClass", pathexe);		
		delete [] pathexe;
		ShowWindow(hwnd, 0);

		char *MainDir = new char[MAX_PATH];
		GetModuleFileName(GetModuleHandle(0), MainDir, MAX_PATH);
		DeleteFileNameFromEndOfPathToFile(MainDir);
		SetCurrentDirectory(MainDir);
		delete [] MainDir;

	#endif
	

	//XMLTable Config;
	if (!Config.LoadFromFile(ConfigFileName.c_str()))
	{
		Log("WARNING", "Can't load XMLTable %s", CONFIG_FILE_NAME);
		return false;
	}
	
	int				wwidth		= atoi((Config.First->Get("WindowWidth"))->GetValue());
	int				wheight		= atoi((Config.First->Get("WindowHeight"))->GetValue());
	int				wbpp		= atoi((Config.First->Get("WindowBpp"))->GetValue());
	char			*wcaption	= (Config.First->Get("WindowCaption"))->GetValue();
					doCalcFps	= !!((Config.First->Get("DoCalcFps"))->Value.compare("true")==0);
					doLimitFps	= !!((Config.First->Get("DoLimitFps"))->Value.compare("true")==0);
					FpsLimit	= atoi((Config.First->Get("FpsLimit"))->GetValue());
	ResourceListPath			= (Config.First->Get("XMLResourcesFilename"))->GetValue();
	ResourceManager.DataPath	= (Config.First->Get("DataPath"))->GetValue();
	ResourceManager.TexturesFldr= (Config.First->Get("TexturesFldr"))->GetValue();
	ResourceManager.FontsFldr	= (Config.First->Get("FontsFldr"))->GetValue();

	CDataLister DataLister;
	DataLister.List();

	SetState(STATE_SCREEN_WIDTH, (void*)wwidth);
	SetState(STATE_SCREEN_HEIGHT, (void*)wheight);
	SetState(STATE_WINDOW_CAPTION, wcaption);

	//SetState(STATE_DO_CALC_FPS, (void*)wdocalcfps);
	//SetState(STATE_DO_LIMIT_FPS, (void*)wdolimitfps);
	//SetState(STATE_FPS_LIMIT, (void*) wFpsLimit);

	SetState(STATE_GUI_KEY_UP, GUIKeyUp);
	SetState(STATE_GUI_KEY_DOWN, GUIKeyDown);
	SetState(STATE_GUI_MOUSE_DOWN, GUIMbDown);
	SetState(STATE_GUI_MOUSE_UP, GUIMbUp);

	window.bpp = 32;
	if (!window.gCreateWindow())
	{
		Log("WARNING", "Window creation failed");
		return false;
	}

	SDL_EnableUNICODE(1);
	gToggleScissor(false);


	TextureManager = CTextureManager::Instance();
	Factory->InitManagers(&UpdateManager, &RenderManager);

//	if (ResourceListPath != NULL && ResourceManager.DataPath != "")


	if (procUserInit != NULL)
		if (!procUserInit())
		{
			Log("WARNING", "User init failed.");
			return false;
		}

	Log("INFO","Initialization success");
	return true;
}

bool CNinja::Suicide()
{
	FontManager->FreeInst("Ninja.cpp");
	TextureManager->FreeInst();
	Factory->FreeInst();
	Log("INFO", "Suicide success");
	return true;
}

#define INPUT_FILTER case SDL_KEYDOWN:case SDL_MOUSEBUTTONDOWN:case SDL_MOUSEBUTTONUP:case SDL_MOUSEMOTION:case SDL_KEYUP:

bool CNinja::ProcessEvents()
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		
		switch(event.type)
		{
			INPUT_FILTER
				for(int i=0;i<EventFuncCount;i++)
					(*EventFunctions[i])(event);
		}
		switch(event.type)
		{
			case SDL_KEYDOWN:
			{
				SDLKey sym = event.key.keysym.sym;
				char ch;
					if ((event.key.keysym.unicode & 0xFF00) == 0 )
						ch = event.key.keysym.unicode & 0xFF;
					if (procGUIGetKeyDown)
						procGUIGetKeyDown(ch, sym);
				if(sym == SDLK_ESCAPE)
				{
					return false;
				}
				keys[sym] = 1;

				break;
			}
			case SDL_MOUSEBUTTONDOWN:
				{
					if (procGUIGetMouseDown)
						procGUIGetMouseDown(event.button.x, event.button.y, event.button.button);
					break;
				}
			case SDL_MOUSEBUTTONUP:
				{
					if (procGUIGetMouseUp)
						procGUIGetMouseUp(event.button.x, event.button.y, event.button.button);
					break;
				}
			case SDL_MOUSEMOTION:
				{
					if (procGUIGetMouseMove)
						procGUIGetMouseMove(event.motion.x, event.motion.y, 0);
					MousePos = Vector2(event.motion.x, window.height - event.motion.y);
					break;
				}
			case SDL_KEYUP:
			{
				SDLKey sym = event.key.keysym.sym;
				keys[sym] = 0;
				if (procGUIGetKeyUp)
					GUIKeyUp(event.key.keysym.unicode & 0xFF, sym);
				break;
			}
			case SDL_VIDEORESIZE:
				{
					window.glResize(event.resize.w, event.resize.h);
					break;
				}
			case SDL_QUIT:
				SDLGLExit(1);
				break;
			default:
			{
				break;
			}
		}
	}
	return true;
}

bool CNinja::Run()
{
	MemChp1();
	//#######################//
	if(!Init())
	{
		Log("ERROR", "Initialization failed");
		SDLGLExit(-1);
		return false;
	}

	Forever
	{
		if (isHaveFocus)
		{	
			if (ProcessEvents() == false) 
					break;
			
			if (LimitFps())
			{				
				if (doCalcFps)
					CalcFps();
				gBeginFrame();
				RenderManager.DrawObjects();
				if (procRenderFunc != NULL)
					procRenderFunc();
				gEndFrame();	
				//##//
				UpdateManager.UpdateObjects();
				if (procUpdateFunc != NULL)
					procUpdateFunc();
				//##//
			}

		}
	}	

	Suicide();
	//##########################//
	MemChp2();
	MemCheck();
	SDLGLExit(0);
	return true;
}

bool CNinja::MidInit()
{
	return true;
}

void CNinja::GetState( int state, void* value )
{
	switch (state)
	{
	case STATE_SCREEN_WIDTH:
		*(int*)value = window.width;
		break;
	case STATE_SCREEN_HEIGHT:
		*(int*)value = window.height;
		break;
	case STATE_MOUSE_X:
		*(float*)value = MousePos.x;
		break;
	case STATE_MOUSE_Y:
		*(float*)value = MousePos.y;
		break;
	case STATE_MOUSE_XY:
		*(Vector2*)value = MousePos;
		break;

	default:
		value = NULL;
		break;
	}
}

bool CNinja::AddEventFunction(EventFunc func)
{
	if (EventFuncCount >= MAX_EVENT_FUNCTIONS)
		return false;
	EventFunctions[EventFuncCount] = func;
	EventFuncCount++;
	return true;
}

int CNinja::CfgGetInt( char* ParamName )
{
	return atoi((Config.First->Get(ParamName))->GetValue());
}

CNinja* CNinja::_instance = 0;
int CNinja::_refcount = 0;

bool CUpdateManager::UpdateObjects()
{
	Reset();
	CNinja *engine = CNinja::Instance();
	CUpdateObject *data = dynamic_cast<CUpdateObject*>(Next());
	while (data)
	{
		data->Update(FIXED_DELTA_TIME); // TODO: подумать что использоваьт: фиксированную дельту или реальную engine->Getdt()
		data = dynamic_cast<CUpdateObject*>(Next());
	}
	engine->FreeInst();
	return true;
}