#include "2de_Engine.h"

#include <IL/il.h>

#include "2de_Core.h"
#include "2de_Sound.h"
#include "2de_GraphicsLow.h"

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif // _WIN32

CEngine::CEngine()	// Переместить все инициализации в вид CEngine::CEngine() : A(AA), B(AB) ...
{
	SetName("Engine main class");
	memset(keys, 0, sizeof(keys));
	doLimitFps			=	false; 
	isHaveFocus			=	true;
	doCalcFps			=	true;
	procUserInit		=	NULL;
	procUserSuicide		=	NULL;
	procFocusLostFunc	=	NULL;
	procFocusGainFunc	=	NULL;
	procUpdateFunc		=	NULL;
	procRenderFunc		=	NULL;
	// temporary, until CConfig created..
	// yes, it's defaults.. developer or maintainer of program should set this by calling CEngine::SetState with STATE_CONFIG_PATH and STATE_CONFIG_NAME
	ConfigFilePath		=	"";
	ConfigFileName		=	"Config.xml";
	EventFuncCount		=	0;
	KeyInputFuncCount	=	0;
	isHaveFocus			=	true;
	userReInit			=	false;
	Initialized			=	false;
}

CEngine::~CEngine()
{

}

void CEngine::CalcFps()
{
	static unsigned long DTime = 0, _llt = SDL_GetTicks(), lt = SDL_GetTicks(), fr = 0;
	
	unsigned long ct = SDL_GetTicks();
	DTime = ct - _llt;
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
	static unsigned long _lt = 0, _llt = 0;
	_lt = SDL_GetTicks();
	if (_lt - _llt >= FpsLimit)
	{				
		_llt = _lt;
		return true;
	}
	return false;
}

void CEngine::SetState(CEngine::EState state, void* value)
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
			FpsLimit = 1000 / (unsigned long)value;
			break;
		case STATE_SCREEN_WIDTH:
			CGLWindow::Instance()->width = (int)value;
			break;
		case STATE_SCREEN_HEIGHT:
			CGLWindow::Instance()->height = (int)value;
			break;
		case STATE_WINDOW_CAPTION:
			CGLWindow::Instance()->caption = (char*)value;
			break;
		case STATE_CONFIG_PATH:
			ConfigFilePath = (char*)value;
			break;
		case STATE_CONFIG_NAME:
			ConfigFileName = (char*)value; // BAD!!!
			// let me tell what is really BAD... EVERYTHING in this GetState/SetState is BAD!!! that's not the way things should be done
			break;
		case STATE_GL_BG_COLOR:
			{
				RGBAf *t = (RGBAf*)value;
				glClearColor(t->x, t->y, t->z, t->w); // BAD!!!  TODO: INcapsulate
			}
			break;
	}
}

bool CEngine::Init()
{
#ifdef _WIN32
	{
// 		HMODULE hmodule = GetModuleHandle(0);
//  		char * pathexe = new char[1024];
// 		GetModuleFileName(hmodule, pathexe, 256);
// 		HWND hwnd = FindWindow("ConsoleWindowClass", pathexe);		
// 		delete [] pathexe;
// 		ShowWindow(hwnd, STATE_HIDE_CONSOLE_WINDOW);  // В ранней версии SDL всегда вылазило окно консоли, потом этот косяк убрали, а мой фикс тут остался

		char *MainDir = new char[MAX_PATH];
		GetModuleFileName(GetModuleHandle(0), MainDir, MAX_PATH);
		DelFNameFromFPath(MainDir);
		SetCurrentDirectory(MainDir);
		delete [] MainDir;
	}
#endif //_WIN32

	CLog::Instance()->SetLogFilePath("Logs/");	// take path from settings or from some system-specific defines
	CLog::Instance()->SetLogName("System");

	Log("INFO", "Working directory is \"%s\"", GetWorkingDir().c_str());

	SDL_putenv("SDL_VIDEO_CENTERED=1");

	CXMLTable Config;
	if (!Config.LoadFromFile(string(ConfigFilePath + ConfigFileName).c_str()))
	{
		Log("ERROR", "Can't load main configuration %s", string(ConfigFilePath + ConfigFileName).c_str());
		return false;
	}
	
	// looks like shit
	int				wwidth		= stoi((Config.First->Get("WindowWidth"))->GetValue());
	int				wheight		= stoi((Config.First->Get("WindowHeight"))->GetValue());
	int				wbpp		= stoi((Config.First->Get("WindowBpp"))->GetValue());
	char			*wcaption	= (Config.First->Get("WindowCaption"))->GetValue();
					doCalcFps	= !!((Config.First->Get("DoCalcFps"))->Value.compare("true")==0);
					doLimitFps	= !!((Config.First->Get("DoLimitFps"))->Value.compare("true")==0);
	SetState(CEngine::STATE_FPS_LIMIT, (void*)stoi((Config.First->Get("FpsLimit"))->GetValue()));

	

	CResourceManager *ResourceManager = CResourceManager::Instance();
	ResourceManager->DataPath	= (Config.First->Get("DataPath"))->GetValue();
	doLoadDefaultResourceList	= !!(Config.First->Get("doLoadDefaultResourceList"))->Value.compare("true")==0;

	SetState(CEngine::STATE_SCREEN_WIDTH, (void*)wwidth);
	SetState(CEngine::STATE_SCREEN_HEIGHT, (void*)wheight);
	SetState(CEngine::STATE_WINDOW_CAPTION, wcaption);

	CFactory::Instance();

	//SetState(STATE_DO_CALC_FPS, (void*)wdocalcfps);
	//SetState(STATE_DO_LIMIT_FPS, (void*)wdolimitfps);
	//SetState(STATE_FPS_LIMIT, (void*) wFpsLimit);
	CRenderManager::Instance()->Camera.SetWidthAndHeight(CGLWindow::Instance()->width, CGLWindow::Instance()->height); // Update camera due to update of wh from config

	CGLWindow::Instance()->bpp = 32;
	if (!CGLWindow::Instance()->gCreateWindow())
	{
		Log("ERROR", "Window creation failed");
		return false;
	}

	ilInit(); // Инициализация DevIL
	CSoundMixer::Instance()->SetMusicVolume(stoi((Config.First->Get("MusicVolume"))->GetValue()));

	SDL_EnableUNICODE(1);

#ifdef I_LIKE_HOW_SDL_KEY_REPEAT_WORKS
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
#endif

	gToggleScissor(false);

	// Default sections:
	ResourceManager->AddSection<CFont>("Fonts");
	ResourceManager->AddSection<CTexture>("Textures");
	ResourceManager->AddSection<CTileset>("Tilesets");
	ResourceManager->AddSection<CSound>("Sounds");
	ResourceManager->AddSection<CMusic>("Music");

	if (doLoadDefaultResourceList)
	{
		if (!ResourceManager->LoadResources())
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
	ilShutDown();
//	ClearLists();
	Log("INFO", "Suicide success");
	return true;
}

#define INPUT_FILTER case SDL_KEYDOWN:case SDL_MOUSEBUTTONDOWN:case SDL_MOUSEBUTTONUP:case SDL_MOUSEMOTION:case SDL_KEYUP:

char TranslateKeyFromUnicodeToChar(const SDL_Event& event)
{
	char TempChar = '\0';
#ifdef _WIN32
	wchar_t  tmp = (event.key.keysym.unicode);							// +русский
	WideCharToMultiByte(CP_ACP, 0, &tmp , 1, &TempChar, 1, NULL, NULL);

#else
	// вообще говоря, наверное даже кроссплатформенно, если хотите - уберите WinAPI и #ifdef
	// хорошо, как-нибудь уберём.
	char *iconv_str_out = SDL_iconv_string("CP1251", "UTF16", (char *) &event.key.keysym.unicode, 2);
	TempChar = iconv_str_out[0];
	SDL_free(iconv_str_out);
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
					(KeyFuncCallers[i]->*KeyInputFunctions[i])(KEY_PRESSED, event.button.button, SDL_GetModState(), 0);				
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
				MousePos = Vector2(event.motion.x, CGLWindow::Instance()->height - event.motion.y);
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
				CGLWindow::Instance()->glResize(event.resize.w, event.resize.h);
				break;
			}
			case SDL_QUIT:
				return false;
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

	while (ProcessEvents())
	{
		try
		{
			if (isHaveFocus)	// Ядрён батон, network, threading итд короче надо этим вопросом заниматься отдельно и вплотную.
			{
				if (LimitFps())
				{		
					if (doCalcFps)
						CalcFps();
					gBeginFrame();
					CRenderManager::Instance()->DrawObjects();
					if (procRenderFunc != NULL)
						procRenderFunc();
					
					CUpdateManager::Instance()->UpdateObjects();
					if (procUpdateFunc != NULL)
						procUpdateFunc(dt);
					// TODO: And look here:(!!!) http://gafferongames.com/game-physics/fix-your-timestep/
				}
				SDL_ShowCursor(0);
				/*тестовый код*/
				int x, y;
				SDL_GetMouseState(&x, &y);
				MousePos = Vector2(x, CGLWindow::Instance()->height - y);
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

				/*PRender.lClr = PRender.pClr = RGBAf(0.8f, 0.8f, 0.8f, 1.0f);
				PRender.lwidth = 4.0f;
				PRender.psize = 4.0f;
				PRender.grArrowL(MousePos + Vector2(10, - 15), MousePos);	// стрелочка, хуле.. но по дефолту оставим круглешок.. */

				PRender.grCircleL(MousePos, 5);
				glEnable(GL_DEPTH_TEST);
				gSetBlendingMode();
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
	#ifdef _WIN32			
				WaitMessage(); // SDL_WaitEvent(NULL);
	#else
				sleep(1);
	#endif //_WIN32
			}
		}
		catch(...)
		{
			Log("ERROR", "A critical error in main loop occured. Exiting");
			throw;
		}
	}	
	Suicide();
	SDL_Quit();

	return true;
}

void CEngine::GetState(CEngine::EState state, void* value)
{
	switch (state)
	{
	case STATE_SCREEN_WIDTH:
		*(int*)value = CGLWindow::Instance()->width;
		break;
	case STATE_SCREEN_HEIGHT:
		*(int*)value = CGLWindow::Instance()->height;
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
		*(unsigned long*)value = FpsLimit;
		break;
	case STATE_FPS_COUNT:
		*(unsigned long*)value = FpsCount;
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
	CXMLTable Config;
	if (!Config.LoadFromFile(string(ConfigFilePath + ConfigFileName).c_str()))
	{
		Log("ERROR", "Can't load main configuration %s", string(ConfigFilePath + ConfigFileName).c_str());
		return false;
	}
	return atoi((Config.First->Get(ParamName))->GetValue());
}

bool CEngine::ClearLists()
{
	// какая к чёрту очистка списков?!
	// 1. объекты сами удаляют указатели на себя из менеджеров в своих деструкторах
	// 2. фабрика при своей смерти вызывает дестркуторы всех объектов (созданных через неё)

	// Не так!!!1!адин!+!+!
	// где-то в тектстовиках я видел шутку про менеджер менеджеров... так вот, он бы реально пригодился тут))
	/*CObjectManager.Clear(); // Почему это не синглтон до сих пор?!
	CRenderManager::Instance()->Reset();
	CUpdateManager::Instance()->Reset();
	CTextureManager::Instance()->Reset();
	CObject *data = (CRenderManager::Instance()->Next());
	while (data)
	{
		if (data && !data->GetListRefCount())
		{
			SAFE_DELETE(data);
		}
		
		data = (CRenderManager::Instance()->Next());
	}	
	data = (CUpdateManager::Instance()->Next());
	while (data)
	{
		if (data && !data->GetListRefCount())
		{
			SAFE_DELETE(data);
		}
			
		data = (CUpdateManager::Instance()->Next());
	}	
	CRenderManager::Instance()->Clear();
	CUpdateManager::Instance()->Clear();

	data = CTextureManager::Instance()->Next();
	while(data)
	{
		if (data && !data->GetListRefCount())
			SAFE_DELETE(data);
		data = CRenderManager::Instance()->Next();
	}
	CRenderManager::Instance()->Clear();*/

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
