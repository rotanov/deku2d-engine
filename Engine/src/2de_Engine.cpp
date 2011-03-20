#include "2de_Engine.h"

//////////////////////////////////////////////////////////////////////////
// CEngine

CEngine CEngine::EngineInstance;

CEngine::CEngine()
{
	memset(Keys, 0, sizeof(Keys));

	Initialized = false;
	Finalizing = false;
	isHaveFocus = true;
	userReInit = false;

	doExitOnEscape = true;
	doLimitFPS = false;
	doCalcFPS = true;

	ProgramName = "Some Deku2D-using program";

	FPSText = NULL;

	StateHandler = NULL;
	SetStateHandler<CAbstractStateHandler>();

	CEnvironment::Paths::SetWorkingDirectory(CEnvironment::Paths::GetExecutablePath());

	CSingletonManager::Init();

	SpatialManager = new CBruteForceSpatialManager();
}

CEngine::~CEngine()
{
	delete StateHandler;
	delete SpatialManager;
}

CEngine* CEngine::Instance()
{
	return &EngineInstance;
}

bool CEngine::Initialize()
{
	CLog::Instance()->SetLogFilePath(CEnvironment::Paths::GetLogPath());
	CLog::Instance()->SetLogName("System");
	Log("INFO", "Working directory is '%s'", CEnvironment::Paths::GetWorkingDirectory().c_str());

	CEnvironment::Variables::Set("SDL_VIDEO_CENTERED", "center");
	
	// config correctness check is not needed - it will just isuue a warning internally - this is not fatal as there's default values..

	CConfig *Config = CConfig::Instance();

	CConfig::CConfigSection VideoSection = Config->Section("Video");

	CGLWindow *Window = CGLWindow::Instance();
	Window->SetWidth(VideoSection["WindowWidth"]);
	Window->SetHeight(VideoSection["WindowHeight"]);
	//Window->BPP = VideoSection["WindowBPP"]; // 32
	Window->SetCaption(VideoSection["WindowCaption"]);
	Window->SetFullscreen(VideoSection["Fullscreen"]);
	doCalcFPS = VideoSection["DoCalcFps"];
	doLimitFPS = VideoSection["DoLimitFps"];
	SetFPSLimit(VideoSection["FpsLimit"]);

	CResourceManager *ResourceManager = CResourceManager::Instance();
	ResourceManager->SetDataPath(Config->Section("Data")["DataPath"]);
	doLoadDefaultResourceList = Config->Section("Data")["doLoadDefaultResourceList"];

	CRenderManager::Instance()->Camera.SetWidthAndHeight(CGLWindow::Instance()->GetWidth(),
		CGLWindow::Instance()->GetHeight()); // Update camera due to update of wh from config


	if (!CGLWindow::Instance()->Create())
	{
		Log("ERROR", "Window creation failed");
		return false;
	}

	ilInit(); // Инициализация DevIL // Captain Obvious IS Obvious.. // If function was called DevILInit() I shall agree with you. But it's fucking "ilInit()"
	CSoundMixer::Instance()->SetMusicVolume(Config->Section("Sound")["MusicVolume"]);

	SDL_EnableUNICODE(1);
	SDL_ShowCursor(0);

	//ToggleKeyRepeat(true); // now can be dynamically switched on/off..

	// Default sections:
	ResourceManager->AddSection<CFont>("Fonts");
	ResourceManager->AddSection<CTexture>("Textures");
	ResourceManager->AddSection<CTileset>("Tilesets");
	ResourceManager->AddSection<CSound>("Sounds");
	ResourceManager->AddSection<CMusic>("Music");
	ResourceManager->AddSection<CScript>("Scripts");
	ResourceManager->AddSection<CPrototype>("Prototypes");
	ResourceManager->AddSection<CModel>("Models");

	if (doLoadDefaultResourceList)
	{
		if (!ResourceManager->LoadResources())
			return false;
	}

	//CFontManager::Instance()->SetCurrentFont("Font");
	CFontManager::Instance()->Init();	// Initialize default font;

	CFactory::Instance(); // Factory should be initialized after all other managers

	Cursor = CFactory::Instance()->New<CMouseCursor>("Mouse cursor");

	//////////////////////////////////////////////////////////////////////////
	//Here goes high level initializations, like default scene as title screen
	//and FPSText
	// TODO: hide it somewhere..
	CPlaceableComponent *FPSTextPlacing = CFactory::Instance()->New<CPlaceableComponent>();
	FPSText = CFactory::Instance()->New<CText>("FPSText");
	FPSText->SetText("FPS: 0");
	FPSTextPlacing->SetLayer(512);
	CUpdateManager::Instance()->RootGameObject->Attach(FPSTextPlacing);
	FPSTextPlacing->Attach(FPSText);

	//	Создание текстуры из памяти
	CTexture *TitleScreenShroomTexture;
 	TitleScreenShroomTexture = CFactory::Instance()->New<CTexture>("TitleScreenShroomTexture");
 	TitleScreenShroomTexture->SetLoadSource(IMAGE_SHROOM_TITLE_DATA, IMAGE_SHROOM_TITLE_SIZE);
	TitleScreenShroomTexture->Load();

	// Создание и установка текущей сцены
	CAbstractScene *TitleScreen = CSceneManager::Instance()->CreateScene();
	CSceneManager::Instance()->SetCurrentScene(TitleScreen);

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
	Log("INFO", "Finalization started");
	Finalizing = true;
	CResourceRefCounterState::DisableRC();
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

bool CEngine::ProcessArguments(int argc, char *argv[])
{
	// return false if you want to stop engine

	CCommandLineArgumentsManager *ArgMan = CCommandLineArgumentsManager::Instance();

	if (!ArgMan->Initialize(argc, argv))
		return false;

	if (ArgMan->GetFlag("version", 'v'))
	{
		cout << "Deku2D, version 0.0.1" << endl << "Copyright 2010  Deku Team" << endl << endl;
		return false;
	}

	CArgumentsConfigMappingsManager *ArgConfigMap = CArgumentsConfigMappingsManager::Instance();

	for (CArgumentsConfigMappingsManager::MappingsIterator it = ArgConfigMap->Begin(); it != ArgConfigMap->End(); ++it)
	{
		if (it->Option) 
		{
			if (!ArgMan->RegisterOption(it->ArgumentLongName, it->ArgumentShortName))
				return false;
		}
	}

	if (!StateHandler->OnArgumentsProcessing())
		return false;

	return true;
}

void CEngine::CalcFPS()
{
	static unsigned long DTime = 0, _llt = SDL_GetTicks(), lt = SDL_GetTicks(), fr = 0;
	
	unsigned long ct = SDL_GetTicks();
	DTime = ct - _llt;
	dt = static_cast<float>(DTime) * 0.001f;
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
			case SDL_KEYDOWN:
			{
				char TempChar = TranslateKeyFromUnicodeToChar(event);
				SDL_keysym keysym = event.key.keysym;

				if (!Keys[keysym.sym])
				{
					CEvent *e = new CEvent;
					e->SetName("KeyDown");
					e->SetData("Char", TempChar);
					e->SetData("Sym", keysym.sym);
					e->SetData("Modifiers", keysym.mod);
					CEventManager::Instance()->TriggerEvent(e);
				}

				CEvent *e = new CEvent;
				e->SetName("KeyPress");
				e->SetData("Char", TempChar);
				e->SetData("Sym", keysym.sym);
				e->SetData("Modifiers", keysym.mod);
				CEventManager::Instance()->TriggerEvent(e);

				Keys[keysym.sym] = true;
				break;
			}
			case SDL_KEYUP:
			{
				if (doExitOnEscape && event.key.keysym.sym == SDLK_ESCAPE)
					return false;			

				char TempChar = TranslateKeyFromUnicodeToChar(event);
				SDL_keysym keysym = event.key.keysym;				

				CEvent *e = new CEvent;
				e->SetName("KeyUp");
				e->SetData("Char", TempChar);
				e->SetData("Sym", keysym.sym);
				e->SetData("Modifiers", keysym.mod);
				CEventManager::Instance()->TriggerEvent(e);

				Keys[keysym.sym] = 0;
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				CEvent *e = new CEvent("MouseDown", NULL);
				e->SetData("X", MousePosition.x);
				e->SetData("Y", MousePosition.y);
				e->SetData("Button", event.button.button);
				e->SetData("Modifiers", SDL_GetModState());
				CEventManager::Instance()->TriggerEvent(e);
				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				CEvent *e = new CEvent("MouseUp", NULL);
				e->SetData("X", MousePosition.x);
				e->SetData("Y", MousePosition.y);
				e->SetData("Button", event.button.button);
				e->SetData("Modifiers", SDL_GetModState());
				CEventManager::Instance()->TriggerEvent(e);
				break;
			}
			case SDL_MOUSEMOTION:
			{
				// Здесь можно раздавать позицию мыши всем попросившим.
				MousePosition = Vector2(event.motion.x, CGLWindow::Instance()->GetHeight() - event.motion.y);
				CPlaceableComponent *CursorPlacing = dynamic_cast<CPlaceableComponent*>(Cursor->GetParent());
				if (CursorPlacing != NULL)
					CursorPlacing->SetPosition(MousePosition);

				CEvent *e = new CEvent("MouseMove", NULL);
				e->SetData("X", MousePosition.x);
				e->SetData("Y", MousePosition.y);
				e->SetData("Modifiers", SDL_GetModState());
				CEventManager::Instance()->TriggerEvent(e);

				break;
			}
			case SDL_ACTIVEEVENT:
			{
				if (event.active.state != SDL_APPMOUSEFOCUS)
				{
					if (isHaveFocus != static_cast<bool>(event.active.gain))
					{
						if (isHaveFocus)
						{
							CSoundMixer::Instance()->PauseMusic(); // TODO: add option to play music in background..
							// CSoundMixer::Instance()->StopAllSound(); // i'm not sure about this..
							StateHandler->OnFocusGain();
						}
						else
						{
							CSoundMixer::Instance()->ResumeMusic();
							StateHandler->OnFocusLost();
						}
					}
					isHaveFocus = static_cast<bool>(event.active.gain);
				}
				break;
			}
			case SDL_VIDEORESIZE:
			{
				CGLWindow::Instance()->SetSize(event.resize.w, event.resize.h);
				CGLWindow::Instance()->Initialize();
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

bool CEngine::Run(int argc, char *argv[])
{
	StateHandler->OnBeforeInitialize();

	if (!ProcessArguments(argc, argv))
	{
		if (CCommandLineArgumentsManager::Instance()->GetErrorState())
			cout << CCommandLineArgumentsManager::Instance()->GetError() << endl << endl;	// maybe incapsulate in something like CEnvironment::Console::Print..

		return false;
	}

	if (!(Initialized = Initialize()))
	{
		Log("ERROR", "Initialization failed");
		SDL_Quit();	// maybe Finalize() to clean-up already initialized systems?
		return false;
	}

	while (ProcessEvents())
	{
		try
		{
			if (isHaveFocus) // not actually. See mainloop issue.
			{
				CResourceManager::Instance()->PerformUnload();
				CFactory::Instance()->CleanUp();
				if (LimitFPS())
				{
					CEventManager::Instance()->TriggerEvent("EveryFrame", NULL);
					CUpdateManager::Instance()->UpdateObjects();
					//CSceneManager::Instance()->Update(dt);
					if (doCalcFPS)
						CalcFPS();
					CRenderManager::Instance()->BeginFrame();
					CRenderManager::Instance()->DrawObjects();
					//CSceneManager::Instance()->Render();
					
					// @todo: And look here:(yet it's about mainloopissue) http://gafferongames.com/game-physics/fix-your-timestep/
				}
				CRenderManager::Instance()->EndFrame();
			}
			else
			{
	#ifdef _WIN32			
				WaitMessage(); // SDL_WaitEvent(NULL);
	#else
				SDL_WaitEvent(NULL); // sleep(1);
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
	// Well, then it's decided, CEngine::pause() should pause the engine,
	// whereas CScene::Pause() should pause the scene.
}

void CEngine::ShutDown()
{
	SDL_Event Event;
	Event.type = SDL_QUIT;
	SDL_PushEvent(&Event);
}

bool CEngine::isFinalizing() const
{
	return Finalizing;
}

void CEngine::SetExitOnEscape(bool AdoExitOnEscape)
{
	doExitOnEscape = AdoExitOnEscape;
}

void CEngine::SetDoLimitFPS(bool AdoLimitFPS)
{
	doLimitFPS = AdoLimitFPS;
}

void CEngine::SetDoCalcFPS(bool AdoCalcFPS)
{
	doCalcFPS = AdoCalcFPS;
}

void CEngine::SetDoKeyRepeat(bool AdoKeyRepeat)
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

unsigned long CEngine::GetFPSLimit() const
{
	return 1000 / FPSLimit;
}

void CEngine::SetFPSLimit(unsigned long AFPSLimit)
{
	FPSLimit = 1000 / AFPSLimit;
}

unsigned long CEngine::GetFPS() const
{
	return FPSCount;
}

void CEngine::SetDoShowFPS(bool AdoShowFPS)
{
	if (FPSText != NULL)
		FPSText->SetVisibility(AdoShowFPS);
}

const Vector2& CEngine::GetMousePosition() const
{
	return MousePosition;
}

bool CEngine::IsKeyDown( const SDLKey& AKey ) const
{
	return Keys[AKey];
}

bool CEngine::IsExitOnEscapeEnabled() const
{
	return doExitOnEscape;
}

bool CEngine::IsLimitFPSEnabled() const
{
	return doLimitFPS;
}

bool CEngine::IsCalcFPSEnabled() const
{
	return doCalcFPS;
}

bool CEngine::IsKeyRepeatEnabled() const
{
	// Don't know how to extract that information from SDL
	return false;
}

bool CEngine::IsShowFPSEnabled( bool AdoShowFPS ) const
{
	return FPSText->GetVisibility();
}
