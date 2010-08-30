#include "2de_Engine.h"

//////////////////////////////////////////////////////////////////////////
// CEngine

CEngine CEngine::EngineInstance;

CEngine::CEngine()
{
	memset(keys, 0, sizeof(keys));

	Initialized = false;
	isHaveFocus = true;
	userReInit = false;

	doExitOnEscape = true;
	doLimitFPS = false; 
	doCalcFPS = true;
	doShowFPS = true;

	ProgramName = "Some Deku2D-using program";

	EventFuncCount = 0;
	KeyInputFuncCount = 0;

	FPSText = NULL;

	StateHandler = NULL;
	SetStateHandler<CAbstractStateHandler>();

	CSingletonManager::Init(); 
}

CEngine::~CEngine()
{
}

CEngine* CEngine::Instance()
{
	return &EngineInstance;
}

bool CEngine::Initialize()
{
	CEnvironment::Paths::SetWorkingDirectory();
	CLog::Instance()->SetLogFilePath("Logs/");	// take path from settings or from some system-specific defines.. or maybe something like CEnvironment::Paths::GetLogsPath()..
	CLog::Instance()->SetLogName("System");
	Log("INFO", "Working directory is \"%s\"", CEnvironment::Paths::GetWorkingDirectory().c_str());

	SDL_putenv("SDL_VIDEO_CENTERED=1");

	// TODO: CConfig
	Config.LoadFromFile(CEnvironment::Paths::GetConfigPath() + ProgramName + ".xml");

	CXMLNode *ConfigRoot = Config.Root.First("Configuration");

	if (ConfigRoot->IsErroneous())
	{
		Log("ERROR", "Can't load main configuration '%s'", string(CEnvironment::Paths::GetConfigPath() + ProgramName + ".xml").c_str());
		return false;
	}
	
	// still looks like shit
	int wwidth = stoi(ConfigRoot->Children.First("Video")->Children.First("WindowWidth")->GetAttribute("value"));
	int wheight = stoi(ConfigRoot->Children.First("Video")->Children.First("WindowHeight")->GetAttribute("value"));
	int wbpp = stoi(ConfigRoot->Children.First("Video")->Children.First("WindowBPP")->GetAttribute("value"));
	string wcaption = ConfigRoot->Children.First("Video")->Children.First("WindowCaption")->GetAttribute("value");
	doCalcFPS = (ConfigRoot->Children.First("Video")->Children.First("DoCalcFps")->GetAttribute("value") == "true");
	doLimitFPS = (ConfigRoot->Children.First("Video")->Children.First("DoLimitFps")->GetAttribute("value") == "true");
	SetFPSLimit(stoi(ConfigRoot->Children.First("Video")->Children.First("FpsLimit")->GetAttribute("value")));
	bool isFullscreen = (ConfigRoot->Children.First("Video")->Children.First("Fullscreen")->GetAttribute("value") == "true");

	// as soon as CConfig will be created, it will look just like this:
	// 	CConfig *Config = CConfig::Instance();
	// 	CGLWindow *Window = CGLWindow::Instance();
	// 	Window->SetWidth(Config->Section("Video")["WindowWidth"]);
	// 	Window->SetHeight(Config->Section("Video")["WindowHeight"]);
	// 	Window->BPP = Config->Section("Video")["WindowBPP"];
	// 	Window->SetCaption(Config->Section("Video")["WindowCaption"]);
	// 	doCalcFps = Config->Section("Video")["DoCalcFps"];
	// 	doLimitFps = Config->Section("Video")["DoLimitFps"];
	// 	etc.
	// at least, I will try to achieve such functionality

	CResourceManager *ResourceManager = CResourceManager::Instance();
	ResourceManager->DataPath = ConfigRoot->Children.First("Data")->Children.First("DataPath")->GetAttribute("value");
	doLoadDefaultResourceList = (ConfigRoot->Children.First("Data")->Children.First("doLoadDefaultResourceList")->GetAttribute("value") == "true");

	CGLWindow::Instance()->SetWidth(wwidth);
	CGLWindow::Instance()->SetHeight(wheight);
	CGLWindow::Instance()->SetCaption(wcaption);

	CRenderManager::Instance()->Camera.SetWidthAndHeight(CGLWindow::Instance()->GetWidth(),
		CGLWindow::Instance()->GetHeight()); // Update camera due to update of wh from config


	CGLWindow::Instance()->BPP = 32;
	CGLWindow::Instance()->Fullscreen = isFullscreen;
	if (!CGLWindow::Instance()->gCreateWindow())
	{
		Log("ERROR", "Window creation failed");
		return false;
	}

	ilInit(); // Инициализация DevIL
	CSoundMixer::Instance()->SetMusicVolume(stoi(ConfigRoot->Children.First("Sound")->Children.First("MusicVolume")->GetAttribute("value")));

	SDL_EnableUNICODE(1);

	//ToggleKeyRepeat(true); // now can be dynamically switched on/off..

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

	//CFontManager::Instance()->SetCurrentFont("Font");
	CFontManager::Instance()->Init();	// Initialize default font;

	CFactory::Instance(); // Factory should be initialized after all other managers

	//////////////////////////////////////////////////////////////////////////
	//Here goes high level initializations, like default scene as title screen
	//and FPSText

	FPSText = CFactory::Instance()->New<CText>("FPSText");
	FPSText->SetText("FPS: 0");
	FPSText->SetLayer(512);

	//	Создание текстуры из памяти
	CTexture *TitleScreenShroomTexture;
	TitleScreenShroomTexture = CFactory::Instance()->New<CTexture>("TitleScreenShroomTexture");
	TitleScreenShroomTexture->LoadTexture(IMAGE_SHROOM_TITLE_WIDTH, IMAGE_SHROOM_TITLE_HEIGHT,
		reinterpret_cast<byte *>(IMAGE_SHROOM_TITLE_DATA));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Создание и установка текущей сцены
	CAbstractScene *TitleScreen = CSceneManager::Instance()->CreateScene();
	CSceneManager::Instance()->SetCurrentScene(TitleScreen);

	// Инициализация текста	
	unsigned int ScrWidth = CGLWindow::Instance()->GetWidth();
	unsigned int ScrHeight = CGLWindow::Instance()->GetHeight();
	CText *TitleText = CFactory::Instance()->New<CText>("txtDeku");
	TitleText->SetText("Deku");
	TitleText->Position = Vector2(ScrWidth * 0.5f + 15.0f, ScrHeight * 0.5f - 22.0f);
	TitleText = CFactory::Instance()->New<CText>("txtTeam");
	TitleText->SetText("team");
	TitleText->Position = Vector2(ScrWidth * 0.5f + 15.0f, ScrHeight * 0.5f - 35.0f);

	// Создание класса CDefaultTutleScreen (в текущей сцене)
	CDefaultTitleScreen *Tscn = CFactory::Instance()->New<CDefaultTitleScreen>("TitleScreenClassForInst");
	Tscn->SetTexture(TitleScreenShroomTexture);
	
	
	if (!StateHandler->OnInitialize())
	{
		Log("ERROR", "State handler OnInitialize has failed");
		return false;
	}

	Initialized = true;
	Log("INFO","Initialization success");
	return true;
}

void CEngine::Finalize()
{
	CFactory::Instance()->DestroyAll();
	CSingletonManager::Instance()->Clear();
	CSingletonManager::Finalize();

#if defined(_DEBUG) && defined(_MSC_VER) && defined(DEKU2D_I_WANT_TO_LOOK_AFTER_MEMORY_LEAKS)
	DumpUnfreed();
#endif

	ilShutDown();
	SDL_Quit();

	StateHandler->OnFinalize();
}

void CEngine::CalcFPS()
{
	static unsigned long DTime = 0, _llt = SDL_GetTicks(), lt = SDL_GetTicks(), fr = 0;
	
	unsigned long ct = SDL_GetTicks();
	DTime = ct - _llt;
	dt = (float)DTime * 0.001f;
	_llt = ct;
	if (ct - lt >= 1000)
	{
		FPSCount = fr * 1000 / (ct - lt);
		lt = ct;
		fr = 0;
	}		
	fr++;
	FPSText->SetText("FPS: " + itos(FPSCount));
}

bool CEngine::LimitFPS()
{
	if (!doLimitFPS)
		return true;
	static unsigned long _lt = 0, _llt = 0;
	_lt = SDL_GetTicks();
	if (_lt - _llt >= FPSLimit)
	{				
		_llt = _lt;
		return true;
	}
	return false;
}


#define INPUT_FILTER case SDL_KEYDOWN:case SDL_MOUSEBUTTONDOWN:\
case SDL_MOUSEBUTTONUP:case SDL_MOUSEMOTION:case SDL_KEYUP:

// possibly move to CEnvironment
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
	while (SDL_PollEvent(&event)) // @todo: Look here!!!!!! :
	//	http://osdl.sourceforge.net/main/documentation/rendering/SDL-inputs.html
	{
		
		switch (event.type)
		{
			INPUT_FILTER
				for (int i = 0; i < EventFuncCount; i++)
					(*EventFunctions[i])(event);
		}
		switch (event.type)
		{
			case SDL_KEYDOWN:
			{
				char TempChar = TranslateKeyFromUnicodeToChar(event);
				SDL_keysym keysym = event.key.keysym;
				for(int i = 0; i < KeyInputFuncCount; i++)
					(KeyFuncCallers[i]->*KeyInputFunctions[i])(KEY_PRESSED, keysym.sym, keysym.mod, TempChar);
				keys[keysym.sym] = 1;
				break;
			}
			case SDL_KEYUP:
			{
				if (doExitOnEscape && event.key.keysym.sym == SDLK_ESCAPE)
					return false;			
				char TempChar = TranslateKeyFromUnicodeToChar(event);
				SDL_keysym keysym = event.key.keysym;				
				for (int i = 0; i < KeyInputFuncCount; i++)
					(KeyFuncCallers[i]->*KeyInputFunctions[i])(KEY_RELEASED, keysym.sym, keysym.mod, TempChar);
				keys[keysym.sym] = 0;
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				for (int i = 0; i < KeyInputFuncCount; i++)
					(KeyFuncCallers[i]->*KeyInputFunctions[i])(KEY_PRESSED, event.button.button, SDL_GetModState(), 0);
				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				for (int i = 0; i < KeyInputFuncCount; i++)
					(KeyFuncCallers[i]->*KeyInputFunctions[i])(KEY_RELEASED, event.button.button, SDL_GetModState(), 0);
				break;
			}
			case SDL_MOUSEMOTION:
			{
				// Здесь можно раздавать позицию мыши всем попросившим.
				MousePos = Vector2(event.motion.x, CGLWindow::Instance()->GetHeight() - event.motion.y);
				//SDL_Delay(2);
				break;
			}
			case SDL_ACTIVEEVENT:
			{
				if (event.active.state != SDL_APPMOUSEFOCUS)
				{
					// бля, от двойного отрицания в глазах рябит... я бы написал (event.active.gain == 1), но по-моему так ещё хуже будет...
					if (isHaveFocus != !!event.active.gain)
					{
						if (!!event.active.gain)
							StateHandler->OnFocusGain();
						else
							StateHandler->OnFocusLost();
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
	StateHandler->OnBeforeInitialize();

	if(!(Initialized = Initialize()))
	{
		Log("ERROR", "Initialization failed");
		SDL_Quit();	// maybe Finalize() to clean-up already initialized systems?
		return false;
	}

	while (ProcessEvents())
	{
		try
		{
			if (isHaveFocus)	// Ядрён батон, network, threading итд короче надо этим вопросом 
								//	заниматься отдельно и вплотную.
			{
				CFactory::Instance()->CleanUp();
				if (LimitFPS())
				{		
					CUpdateManager::Instance()->UpdateObjects();
					CSceneManager::Instance()->Update(dt);
					if (doCalcFPS)
						CalcFPS();
					gBeginFrame();
					CRenderManager::Instance()->DrawObjects();
					CSceneManager::Instance()->Render();
					
					// @todo: And look here:(!!!) http://gafferongames.com/game-physics/fix-your-timestep/
				}
				SDL_ShowCursor(0);
				/*тестовый код*/	// надо сделать абстрактный класс CTestCode с единственной виртуальной функцией Execute
							// и засовывать весь такой тестовый код в его наследников.. это такая - полу-шутка, полу-серьёзность..
				int x, y;
				SDL_GetMouseState(&x, &y);
				MousePos = Vector2(x, CGLWindow::Instance()->GetHeight() - y);
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
				PRender.grArrowL(MousePos + Vector2(10, - 15), MousePos);	// стрелочка, хуле..
				но по дефолту оставим круглешок.. */

				PRender.grCircleL(MousePos, 5);
				glEnable(GL_DEPTH_TEST);
				gSetBlendingMode();
				gEndFrame();	

				/**/
				// ZOMG There wasn't other choice, the next step is to put it all into separate thread.
				// Or pseudo-thread.

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
	// + идея от меня - можно пытаться предсказывать движение курсора, т.е. где он окажется,
	//	пока мы рисуем кадры под 60FPS
	// Экстраполяция же.

	//  перетяните уже кто-нибудь что-нибудь куда-нибудь... такие главные функции как Run()
	//	и Init() должны быть короткими, лаконичными и состоять в основном из вызовов других функций..
	//	а тут блин даже "документация" валяется..

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
		catch (...)
		{
			Log("ERROR", "An unhandled exception occured in main loop. Exiting");
			throw;
		}
	}	

	StateHandler->OnBeforeFinalize();
	Finalize();

	return true;
}

void CEngine::Pause()
{
	// PAUSE; lol i dunno wich level to pause here
	// i mean full engine level pause or something else
	// i think more about first
	// 	i suppose that it may be pause of events processing
}

void CEngine::ShutDown()
{
	SDL_Event Event;
	Event.type = SDL_QUIT;
	SDL_PushEvent(&Event);
}

bool CEngine::AddEventFunction(EventFunc func)
{
	if (EventFuncCount >= MAX_EVENT_FUNCTIONS)
		return false;
	EventFunctions[EventFuncCount] = func;
	EventFuncCount++;
	return true;
}

bool CEngine::AddKeyInputFunction(KeyInputFunc AKeyInputFunction, CObject* AKeyFuncCaller)
{
	if (KeyInputFuncCount >= MAX_KEY_INPUT_FUNCTIONS)
		return false;
	KeyInputFunctions[KeyInputFuncCount] = AKeyInputFunction;
	KeyFuncCallers[KeyInputFuncCount] = AKeyFuncCaller;
	KeyInputFuncCount++;
	return true;
}

void CEngine::ToggleExitOnEscape(bool AdoExitOnEscape)
{
	doExitOnEscape = AdoExitOnEscape;
}

void CEngine::ToggleLimitFPS(bool AdoLimitFPS)
{
	doLimitFPS = AdoLimitFPS;
}

void CEngine::ToggleCalcFPS(bool AdoCalcFPS)
{
	doCalcFPS = AdoCalcFPS;
}

void CEngine::ToggleKeyRepeat(bool AdoKeyRepeat)
{
	int RepeatDelay = AdoKeyRepeat ? SDL_DEFAULT_REPEAT_DELAY : 0;
	SDL_EnableKeyRepeat(RepeatDelay, SDL_DEFAULT_REPEAT_INTERVAL);
}

float CEngine::GetDeltaTime() const
{
	return dt;
}

string CEngine::GetProgramName() const
{
	return ProgramName;
}

void CEngine::SetProgramName(const string &AProgramName)
{
	ProgramName = AProgramName; 
}

// well, getter and setter do different things.. something wrong here..
unsigned long CEngine::GetFPSLimit() const
{
	return FPSLimit;
}

void CEngine::SetFPSLimit(unsigned long AFPSLimit)
{
	FPSLimit = 1000 / AFPSLimit;
}

unsigned long CEngine::GetFPS() const
{
	return FPSCount;
}
