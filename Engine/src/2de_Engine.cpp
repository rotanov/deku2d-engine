#include "2de_Engine.h"

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif //_WIN32

//-------------------------------------------//
//				Ninja stuff					 //
//-------------------------------------------//

//CEngine

CEngine::CEngine()
{
	CreateLogFile("System.log");
	SetName("Engine main class");
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
	Factory = CFactory::Instance();
	FontManager = CFontManager::Instance();
	window = CGLWindow::Instance();
	//_instance = NULL;
	//_refcount = 0;
	ConfigFileName = CONFIG_FILE_NAME;
	ConfigFileName += "configuration.xml";
	EventFuncCount = 0;
	KeyInputFuncCount = 0;
	isHaveFocus = 1;
	userReInit = false;
	Initialized = false;
}
CEngine::~CEngine(){}

CEngine* CEngine::Instance()
{
// 	if (_instance == NULL)
// 	{
// 		_instance = new CEngine;
// 	}
// 	_refcount++;
	if(!_instance)
	{
		_instance = new CEngine;
		SingletoneKiller.AddObject(_instance);
	}
	return _instance;
}

void CEngine::CalcFps()
{
	static DWORD DTime = 0, _llt = SDL_GetTicks(), lt = SDL_GetTicks(), fr = 0;
	
	DWORD ct = SDL_GetTicks();
	DTime = ct-_llt;
	dt = (float)DTime * 0.001f;
	_llt = ct;
	if (ct-lt >= 1000)
	{
		FpsCount = fr*1000/(ct-lt);
		lt = ct;
		fr = 0;
	}		
	fr++;
}

bool CEngine::LimitFps()
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


void CEngine::SetState(int state, void* value)
	{
		switch(state)
		{
			case STATE_USER_INIT_FUNC:
				procUserInit = (Callback) value;
				if (Initialized && procUserInit != NULL)
				{
					// Так как минимум один раз пользовательская инициализация уже была установлена,
					// а следовательно вызвана, то здесь надо вообще всё остановить и подчистить.
					// И потом переинициализировать всё что нужно и пользовательскую инициализацию
//					ClearLists();  
					if (!(Initialized = procUserInit()))
						Log("ERROR", "Попытка выполнить пользовательскую инициализацию заново провалилась.");
				}
				break;
			case STATE_UPDATE_FUNC:
				procUpdateFunc = (UpdateProc) value;
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
				FpsLimit = 1000 / (DWORD)value;
				break;
			case STATE_SCREEN_WIDTH:
				window->width = (int)value;
				break;
			case STATE_SCREEN_HEIGHT:
				window->height = (int)value;
				break;
			case STATE_WINDOW_CAPTION:
				window->caption = (char*)value;
				break;
			case STATE_CONFIG_NAME:
				{
					ConfigFileName = CONFIG_FILE_NAME;
					ConfigFileName += (char*)value; // BAD!!! 
				}
			case STATE_GL_BG_COLOR:
				{
					RGBAf *t = (RGBAf*)value;
					glClearColor(t->x, t->y, t->z, t->w); // BAD!!!  TODO: INcapsulate
				}
		}
	}

bool CEngine::Init()
{
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN		
	{
		HMODULE hmodule = GetModuleHandle("Ninja Engine.exe");
 		char * pathexe = new char[1024];
		GetModuleFileName(hmodule, pathexe, 256);
		HWND hwnd = FindWindow("ConsoleWindowClass", pathexe);		
		delete [] pathexe;
		ShowWindow(hwnd, STATE_HIDE_CONSOLE_WINDOW);  // В ранней версии SDL всегда вылазило окно консоли, потом этот косяк убрали, а мой фикс тут остался

		char *MainDir = new char[MAX_PATH];
		GetModuleFileName(GetModuleHandle(0), MainDir, MAX_PATH);
		DelFNameFromFPath(MainDir);
		SetCurrentDirectory(MainDir);
		delete [] MainDir;
	}	
#endif //_WIN32

	if (!Config.LoadFromFile(ConfigFileName.c_str()))
	{
		Log("ERROR", "Can't load main configuration %s", CONFIG_FILE_NAME);
		return false;
	}
	
	// looks like shit
	int				wwidth		= atoi((Config.First->Get("WindowWidth"))->GetValue());
	int				wheight		= atoi((Config.First->Get("WindowHeight"))->GetValue());
	int				wbpp		= atoi((Config.First->Get("WindowBpp"))->GetValue());
	char			*wcaption	= (Config.First->Get("WindowCaption"))->GetValue();
					doCalcFps	= !!((Config.First->Get("DoCalcFps"))->Value.compare("true")==0);
					doLimitFps	= !!((Config.First->Get("DoLimitFps"))->Value.compare("true")==0);
	SetState(STATE_FPS_LIMIT, (void*)atoi((Config.First->Get("FpsLimit"))->GetValue()));
	ResourceListPath			= (Config.First->Get("XMLResourcesFilename"))->GetValue();
	ResourceManager.DataPath	= (Config.First->Get("DataPath"))->GetValue();
	doLoadDefaultResourceList	= !!(Config.First->Get("doLoadDefaultResourceList"))->Value.compare("true")==0;

	SetState(STATE_SCREEN_WIDTH, (void*)wwidth);
	SetState(STATE_SCREEN_HEIGHT, (void*)wheight);
	SetState(STATE_WINDOW_CAPTION, wcaption);

	//SetState(STATE_DO_CALC_FPS, (void*)wdocalcfps);
	//SetState(STATE_DO_LIMIT_FPS, (void*)wdolimitfps);
	//SetState(STATE_FPS_LIMIT, (void*) wFpsLimit);
	RenderManager.Camera.SetWidthAndHeight(CGLWindow::Instance()->width, CGLWindow::Instance()->height); // Update camera due to update of wh from config

	window->bpp = 32;
	if (!window->gCreateWindow())
	{
		Log("ERROR", "Window creation failed");
		return false;
	}

	SDL_EnableUNICODE(1);
	gToggleScissor(false);


	TextureManager = CTextureManager::Instance();
	Factory->InitManagers(&UpdateManager, &RenderManager);

	if (doLoadDefaultResourceList)
	{
		if (!ResourceManager.LoadResources())
			return false;
	}


	if (procUserInit != NULL)
		if (!procUserInit())
		{
			Log("ERROR", "User init failed.");
			return false;
		}

	Initialized = true;
	Log("INFO","Initialization success");
	return true;
}

bool CEngine::Suicide()
{
	ClearLists();
	Log("INFO", "Suicide success");
	return true;
}

#define INPUT_FILTER case SDL_KEYDOWN:case SDL_MOUSEBUTTONDOWN:case SDL_MOUSEBUTTONUP:case SDL_MOUSEMOTION:case SDL_KEYUP:

char TranslateKeyFromUnicodeToChar(const SDL_Event& event)
{
	SDLKey sym = event.key.keysym.sym;
	char TempChar;
#ifdef _WIN32
	wchar_t  tmp = (event.key.keysym.unicode);							// +русский
	WideCharToMultiByte(CP_ACP, 0, &tmp , 1, &TempChar, 1, NULL, NULL);

#else
	if ((event.key.keysym.unicode & 0xFF80) == 0 )  // только английский
		TempChar = event.key.keysym.unicode & 0x7F;
#endif //_WIN32
	return TempChar;
}

bool CEngine::ProcessEvents()
{
	SDL_Event event;
	while(SDL_PollEvent(&event)) // TODO: Look here!!!!!! http://osdl.sourceforge.net/main/documentation/rendering/SDL-inputs.html
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
				char TempChar = TranslateKeyFromUnicodeToChar(event);
				SDL_keysym keysym = event.key.keysym;
				for(int i = 0; i < KeyInputFuncCount; i++)
					(KeyFuncCallers[i]->*KeyInputFunctions[i])(KEY_PRESSED, keysym.sym, keysym.mod, TempChar);				
				// Глобальная рекция на escape! Слишком большой хардкод, но пока сойдёт. Потом - либо вывести в опцию, либо убрать и предоставить программисту право выбора
				//if(keysym.sym == SDLK_ESCAPE)	
				//		return false;
				keys[keysym.sym] = 1;
				break;
			}
			case SDL_KEYUP:
			{
				char TempChar = TranslateKeyFromUnicodeToChar(event);
				SDL_keysym keysym = event.key.keysym;				
				for(int i=0;i<KeyInputFuncCount;i++)
					(KeyFuncCallers[i]->*KeyInputFunctions[i])(KEY_RELEASED, keysym.sym, keysym.mod, TempChar);				
				keys[keysym.sym] = 0;
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				for(int i=0;i<KeyInputFuncCount;i++)
					(KeyFuncCallers[i]->*KeyInputFunctions[i])(KEY_RELEASED, event.button.button, SDL_GetModState(), 0);				
				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				for(int i=0;i<KeyInputFuncCount;i++)
					(KeyFuncCallers[i]->*KeyInputFunctions[i])(KEY_RELEASED, event.button.button, SDL_GetModState(), 0);				
				break;
			}
			case SDL_MOUSEMOTION:
			{
				// Здесь можно раздавать позицию мыши всем попросившим.
				MousePos = Vector2(event.motion.x, window->height - event.motion.y);
				//SDL_Delay(2);
				break;
			}
			case SDL_ACTIVEEVENT:
			{
				if (event.active.state != SDL_APPMOUSEFOCUS)
				{
					if (isHaveFocus != !!event.active.gain)
					{
						if (!!event.active.gain)
							if (procFocusGainFunc)
								procFocusGainFunc();
							else;
						else
							if (procFocusLostFunc)
								procFocusLostFunc();
					}
					isHaveFocus = !!event.active.gain;
				}
				break;
			}
			case SDL_VIDEORESIZE:
			{
				window->glResize(event.resize.w, event.resize.h);
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

bool CEngine::Run()
{
	if(!(Initialized = Init()))
	{
		Log("ERROR", "Initialization failed");
		SDLGLExit(-1);
		return false;
	}

	Forever
	{
		if (ProcessEvents() == false) 
			break;
		if (isHaveFocus)	// Ядрён батон, network, threading итд короче надо этим вопросом заниматься отдельно и вплотную.
		{
			if (LimitFps())
			{		
				if (doCalcFps)
					CalcFps();
				gBeginFrame();
				RenderManager.DrawObjects();
				if (procRenderFunc != NULL)
					procRenderFunc();
				
				UpdateManager.UpdateObjects();
				if (procUpdateFunc != NULL)
					procUpdateFunc(dt);
				// TODO: And look here:(!!!) http://gafferongames.com/game-physics/fix-your-timestep/
			}
			SDL_ShowCursor(0);
			/*тестовый код*/
			int x, y;
			SDL_GetMouseState(&x, &y);
			MousePos = Vector2(x, window->height - y);
			const RGBAf				COLOR_FIRST(.4f, .4f, .4f, 1.0f);
			const RGBAf				COLOR_SECOND(.5f, .5f, .6f, 1.0f);
			const RGBAf				COLOR_THIRD(0.6f, 0.7f, 0.8f, 0.5f);
			const RGBAf				COLOR_FOURTH(0.9f, 0.8f, 0.2f, 1.0f);
			CPrimitiveRender PRender;
			PRender.lClr = COLOR_FIRST;
			PRender.sClr = COLOR_THIRD;
			PRender.pClr = COLOR_FIRST;
			glLoadIdentity();
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);		
			glBlendFunc(GL_SRC_ALPHA,GL_ONE);
			PRender.grCircleL(MousePos, 5);
			gEndFrame();	

			/**/
			// ZOMG There wasn't other choice, the next step is to put it all into separate thread. Or pseudo-thread.

// 			как уже подсказали, нужно 
// 				> Можно вынести опрос координат курсора в отдельный поток. 
// 
// 				но применительно для SDL, можно установить глобальный фильтр обработчика где будет, вызываться твоя функция отрисовки курсора (я делал именно так, и не тормозит)
// 
// 				SDL_SetEventFilter(GlobalFilterEvents);
// 
// 
// 			int GlobalFilterEvents(const SDL_Event *event) 
// 			{ 
// 				if(SDL_MOUSEMOTION == event->type) 
// 					RedrawCursor(event->motion.x, event->motion.y);
// 
// 				return 1; 
// 			}
// 
// 			это не дословный код но общая идея именно такая.
//------------------
// + идея от меня - можно пытаться предсказывать движение курсора, т.е. где он окажется, пока мы рисуем кадры под 60FPS

		}
		else
		{
			//WaitMessage();
#ifdef _WIN32
			Sleep(1);
#else
			sleep(1);
#endif //_WIN32
		}
	}	
#ifdef _DEBUG
	CObjectManager.DumpToLog();
#endif
	Suicide();	// БЛЯБЛЯБЯЛЯБЛЯБЛЯБЛЯ МЫ СЮДА НЕ ПОПАДАЕМ
	SDLGLExit(0); // Если мы попадаем сюда, то в место после вызова Run() мы уже не попадём. Это проблема, я думаю, надо что-то другое придумать.
	return true;
}

bool CEngine::MidInit() // Для чего эта ф-я?
{
	return true;
}

void CEngine::GetState(int state, void* value)
{
	switch (state)
	{
	case STATE_SCREEN_WIDTH:
		*(int*)value = window->width;
		break;
	case STATE_SCREEN_HEIGHT:
		*(int*)value = window->height;
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
	case STATE_FPS_LIMIT:
		*(DWORD*)value = FpsLimit;
		break;
	case STATE_FPS_COUNT:
		*(DWORD*)value = FpsCount;
		break;
	case STATE_DELTA_TIME:
		*(float*)value = dt;
		break;
	default:
		value = NULL;
		break;
	}
}

bool CEngine::AddEventFunction(EventFunc func)
{
	if (EventFuncCount >= MAX_EVENT_FUNCTIONS)
		return false;
	EventFunctions[EventFuncCount] = func;
	EventFuncCount++;
	return true;
}

int CEngine::CfgGetInt( char* ParamName )
{
	return atoi((Config.First->Get(ParamName))->GetValue());
}

bool CEngine::ClearLists()
{
	// Не так!!!1!адин!+!+!
	CObjectManager.Clear();
	RenderManager.Reset();
	UpdateManager.Reset();
	TextureManager->Reset();
	CObject *data = (RenderManager.Next());
	while (data)
	{
		if (data && !data->GetListRefCount())
		{
			SAFE_DELETE(data);
		}
		
		data = (RenderManager.Next());
	}	
	data = (UpdateManager.Next());
	while (data)
	{
		if (data && !data->GetListRefCount())
		{
			SAFE_DELETE(data);
		}
			
		data = (UpdateManager.Next());
	}	
	RenderManager.Clear();
	UpdateManager.Clear();

	data = TextureManager->Next();
	while(data)
	{
		if (data && !data->GetListRefCount())
			SAFE_DELETE(data);
		data = RenderManager.Next();
	}
	RenderManager.Clear();

	return true;
}

bool CEngine::AddKeyInputFunction( KeyInputFunc AKeyInputFunction, CObject* AKeyFuncCaller)
{
	if (KeyInputFuncCount >= MAX_KEY_INPUT_FUNCTIONS)
		return false;
	KeyInputFunctions[KeyInputFuncCount] = AKeyInputFunction;
	KeyFuncCallers[KeyInputFuncCount] = AKeyFuncCaller;
	KeyInputFuncCount++;
	return true;
}
CEngine *CEngine::_instance = NULL;
//int CEngine::_refcount = 0;

bool CUpdateManager::UpdateObjects()
{
	Reset();
	CEngine *engine = CEngine::Instance();
	CUpdateObject *data = dynamic_cast<CUpdateObject*>(Next());
	while (data)
	{
		if (!data->Active)
			continue;
		// FIXED_DELTA_TIME
		float dt = 0;
		CEngine::Instance()->GetState(STATE_DELTA_TIME, &dt);
		data->Update(dt); // TODO: подумать что использоваьт: фиксированную дельту или реальную engine->Getdt()
		data = dynamic_cast<CUpdateObject*>(Next());
	}
	return true;
}
