#include "2de_Engine.h"

#include <IL/il.h>

namespace Deku2D
{
	//////////////////////////////////////////////////////////////////////////
	// CEngine

	CEngine CEngine::EngineInstance;

	CEngine::CEngine()
	{
		BigEngineLock = SDL_CreateMutex();

		SetInitialValues();

		IdleWhenInBackground = true;

		ProgramName = "Some Deku2D-using program";

		StateHandler = NULL;
		SetStateHandler<CAbstractStateHandler>();

		Environment::Paths::SetWorkingDirectory(Environment::Paths::GetExecutablePath());

		CSingletonManager::Init();

		SpatialManager = new CBruteForceSpatialManager();
	}

	CEngine::~CEngine()
	{
		delete StateHandler;
		delete SpatialManager;
		CSingletonManager::Finalize();

		SDL_DestroyMutex(BigEngineLock);
	}

	CEngine* CEngine::Instance()
	{
		return &EngineInstance;
	}

	// possibly move to CEnvironment
	char TranslateKeyFromUnicodeToChar(const SDL_Event& event)
	{
		char TempChar = '\0';
	#ifdef _WIN32
		wchar_t  tmp = (event.key.keysym.unicode);							// +русский
		WideCharToMultiByte(CP_ACP, 0, &tmp , 1, &TempChar, 1, NULL, NULL);

	#elif HAVE_ICONV
		// вообще говоря, наверное даже кроссплатформенно, если хотите - уберите WinAPI и #ifdef
		// хорошо, как-нибудь уберём.
		char *iconv_str_out = SDL_iconv_string("CP1251", "UTF-16", (char *) &event.key.keysym.unicode, 2);
		TempChar = iconv_str_out[0];
		SDL_free(iconv_str_out);
	#else
		if ((event.key.keysym.unicode & 0xFF80) == 0)
			TempChar = event.key.keysym.unicode & 0x7F;
	#endif //_WIN32
		return TempChar;
	}

	bool CEngine::Run(int argc, char *argv[])
	{
		SetInitialValues();

		StateHandler->OnBeforeInitialize();

		if (!ProcessArguments(argc, argv))
		{
			if (CommandLineArgumentsManager->GetErrorState())
				cout << CommandLineArgumentsManager->GetError() << endl << endl;	// maybe incapsulate in something like CEnvironment::Console::Print..

			return false;
		}

		if (!(Initialized = Initialize()))
		{
			Log("ERROR", "Initialization failed");
			SDL_Quit();	// maybe Finalize() to clean-up already initialized systems?
			return false;
		}

		// KLUDGE: clear SDL event queue to prevent some strange behavior of window resizing on platforms that use X11..
		SDL_Event e;
		while (SDL_PollEvent(&e));

		Running = true;

		while (ProcessEvents())
		{
			Lock();
			try
			{
				if (isHaveFocus) // not actually. See main loop issue.
				{
					ResourceManager->PerformUnload();
					Factory->CleanUp();
					if (LimitFPS())
					{
						EventManager->TriggerEvent("EveryFrame", NULL);
						SpatialManager->Update();
						SpatialManager->Clear();
						//SceneManager->Update(dt);
						if (doCalcFPS)
							CalcFPS();
						RenderManager->BeginFrame();
						RenderManager->DrawObjects();
						//SceneManager->Render();
						
						// @todo: And look here:(yet it's about mainloopissue) http://gafferongames.com/game-physics/fix-your-timestep/
					}
					RenderManager->EndFrame();
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
			Unlock();
		}

		Running = false;

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

	bool CEngine::isRunning() const
	{
		return Running;
	}

	bool CEngine::isFinalizing() const
	{
		return Finalizing;
	}

	void CEngine::Lock()
	{
		SDL_mutexP(BigEngineLock);
	}

	void CEngine::Unlock()
	{
		SDL_mutexV(BigEngineLock);
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
		// TODO: store the state of key repeating in the engine itself (i don't know, though, is there any default state?)
		return false;
	}

	bool CEngine::IsShowFPSEnabled() const
	{
		return FPSText ? FPSText->GetVisibility() : false;
	}

	bool CEngine::IsIdleWhenInBackground() const
	{
		return IdleWhenInBackground;
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

	void CEngine::SetDoShowFPS(bool AdoShowFPS)
	{
		if (FPSText != NULL)
			FPSText->SetVisibility(AdoShowFPS);
	}

	void CEngine::SetIdleWhenInBackground(bool AIdleWhenInBackground)
	{
		IdleWhenInBackground = AIdleWhenInBackground;
	}

	string CEngine::GetProgramName() const
	{
		return ProgramName;
	}

	void CEngine::SetProgramName(const string &AProgramName)
	{
		ProgramName = AProgramName;
	}

	float CEngine::GetDeltaTime() const
	{
		return dt;
	}

	unsigned long CEngine::GetFPS() const
	{
		return FPSCount;
	}

	unsigned long CEngine::GetFPSLimit() const
	{
		return 1000 / FPSLimit;
	}

	void CEngine::SetFPSLimit(unsigned long AFPSLimit)
	{
		FPSLimit = 1000 / AFPSLimit;
	}

	bool CEngine::IsKeyDown(const SDLKey& AKey) const
	{
		return Keys[AKey];
	}

	const Vector2& CEngine::GetMousePosition() const
	{
		return MousePosition;
	}

	bool CEngine::Initialize()
	{
		SLog->SetLogFilePath(Environment::Paths::GetLogPath());
		SLog->SetLogName("System");
		Log("INFO", "Working directory is '%s'", Environment::Paths::GetWorkingDirectory().c_str());

		Environment::Variables::Set("SDL_VIDEO_CENTERED", "center");

		SLog->SetLogLevel(Config->Section("Data")["LogLevel"]);

		CConfig::CConfigSection VideoSection = Config->Section("Video");

		CGLWindow *Window = GLWindow.Instance();
		Window->SetWidth(VideoSection["WindowWidth"]);
		Window->SetHeight(VideoSection["WindowHeight"]);
		//Window->BPP = VideoSection["WindowBPP"]; // 32
		Window->SetCaption(VideoSection["WindowCaption"]);
		Window->SetFullscreen(VideoSection["Fullscreen"]);
		doCalcFPS = VideoSection["DoCalcFps"];
		doLimitFPS = VideoSection["DoLimitFps"];
		SetFPSLimit(VideoSection["FpsLimit"]);

		ResourceManager->SetDataPath(Config->Section("Data")["DataPath"]);
		doLoadDefaultResourceList = Config->Section("Data")["doLoadDefaultResourceList"];

		// Update camera due to update of wh from config
		RenderManager->Camera.SetWidthAndHeight(GLWindow->GetWidth(), GLWindow->GetHeight());


		if (!GLWindow->Create())
		{
			Log("ERROR", "Window creation failed");
			return false;
		}

		ilInit(); // Инициализация DevIL // Captain Obvious IS Obvious.. // If function was called DevILInit() I shall agree with you. But it's fucking "ilInit()" // You think "il" doesn't clearly denote "Image Library"?
		SoundMixer->SetMusicVolume(Config->Section("Sound")["MusicVolume"]);

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

		//FontManager->SetCurrentFont("Font");
		FontManager->Init();	// Initialize default font;

		Factory.Instance(); // Factory should be initialized after all other managers

		CScript *mainScript = Factory->Get<CScript>("Main");
		if (mainScript == NULL)
			mainScript = Factory->Get<CScript>("main");
		if (mainScript == NULL)
		{
			Log("ERROR", "Unable to load Main.lua");
			return false;
		}
		LuaVirtualMachine->RunScript(mainScript);	

		RootGameObject = dynamic_cast<CPlaceableComponent *>(Factory->CreateByName("PlaceableComponent", "Root"));

		CGameObject* Cursor = dynamic_cast<CGameObject*>(Factory->CreateByName("MouseProto", "Mouse cursor"));
		if (Cursor != NULL)
			RootGameObject->Attach(Cursor);
		else
			SDL_ShowCursor(1);

		//////////////////////////////////////////////////////////////////////////
		//Here goes high level initializations, like default scene as title screen
		//and FPSText
		// TODO: hide it somewhere..
		CPlaceableComponent *FPSTextPlacing = Factory->New<CPlaceableComponent>();
		FPSTextPlacing->GetTransformation().SetTranslation(Vector2(200.0f, 300.0f));
		FPSText = Factory->New<CText>("FPSText");
		FPSText->SetText("FPS: 0");
		FPSTextPlacing->SetLayer(512);
		RootGameObject->Attach(FPSTextPlacing);
		FPSTextPlacing->Attach(FPSText);

		//	Создание текстуры из памяти
		CTexture *TitleScreenShroomTexture;
 		TitleScreenShroomTexture = Factory->New<CTexture>("TitleScreenShroomTexture");
		TitleScreenShroomTexture->SetLoadSource(reinterpret_cast<unsigned char*>(Const::Graphics::IMAGE_SHROOM_TITLE_DATA), Const::Graphics::IMAGE_SHROOM_TITLE_SIZE);
		TitleScreenShroomTexture->Load();

		// Создание и установка текущей сцены
		CAbstractScene *TitleScreen = SceneManager->CreateScene();
		SceneManager->SetCurrentScene(TitleScreen);

		// Создание класса CDefaultTutleScreen (в текущей сцене)
		CDefaultTitleScreen *Tscn = Factory->New<CDefaultTitleScreen>("TitleScreenClassForInst");
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
		Factory->DestroyAll();
		CSingletonManager::Instance()->Clear();

	#if defined(_DEBUG) && defined(_MSC_VER) && defined(DEKU2D_I_WANT_TO_LOOK_AFTER_MEMORY_LEAKS)
		DumpUnfreed();
	#endif

		ilShutDown();
		SDL_Quit();

		StateHandler->OnFinalize();

		Finalizing = false;
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
						EventManager->TriggerEvent(e);
					}

					CEvent *e = new CEvent;
					e->SetName("KeyPress");
					e->SetData("Char", TempChar);
					e->SetData("Sym", keysym.sym);
					e->SetData("Modifiers", keysym.mod);
					EventManager->TriggerEvent(e);

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
					EventManager->TriggerEvent(e);

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
					EventManager->TriggerEvent(e);
					break;
				}
				case SDL_MOUSEBUTTONUP:
				{
					CEvent *e = new CEvent("MouseUp", NULL);
					e->SetData("X", MousePosition.x);
					e->SetData("Y", MousePosition.y);
					e->SetData("Button", event.button.button);
					e->SetData("Modifiers", SDL_GetModState());
					EventManager->TriggerEvent(e);
					break;
				}
				case SDL_MOUSEMOTION:
				{
					MousePosition = Vector2(event.motion.x, GLWindow->GetHeight() - event.motion.y);
					CEvent *e = new CEvent("MouseMove", NULL);
					e->SetData("X", MousePosition.x);
					e->SetData("Y", MousePosition.y);
					e->SetData("Modifiers", SDL_GetModState());
					EventManager->TriggerEvent(e);
					break;
				}
				case SDL_ACTIVEEVENT:
				{
					if (event.active.state != SDL_APPMOUSEFOCUS && IdleWhenInBackground)
					{
						if (isHaveFocus != static_cast<bool>(event.active.gain))
						{
							if (isHaveFocus)
							{
								SoundMixer->PauseMusic(); // TODO: add option to play music in background..
								// SoundMixer->StopAllSound(); // i'm not sure about this..
								StateHandler->OnFocusGain();
							}
							else
							{
								SoundMixer->ResumeMusic();
								StateHandler->OnFocusLost();
							}
						}
						isHaveFocus = static_cast<bool>(event.active.gain);
					}
					break;
				}
				case SDL_VIDEORESIZE:
				{
					GLWindow->SetSize(event.resize.w, event.resize.h);
					GLWindow->Initialize();
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

	bool CEngine::ProcessArguments(int argc, char *argv[])
	{
		// return false if you want to stop engine

		CCommandLineArgumentsManager *ArgMan = CommandLineArgumentsManager.Instance();

		if (!ArgMan->Initialize(argc, argv))
			return false;

		if (ArgMan->GetFlag("version", 'v'))
		{
			cout << "Deku2D, version 0.0.1" << endl << "Copyright 2009-2011  Deku Team" << endl << endl;
			return false;
		}

		CArgumentsConfigMappingsManager *ArgConfigMap = ArgumentsConfigMappingsManager.Instance();

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

	void CEngine::SetInitialValues()
	{
		memset(Keys, 0, sizeof(Keys));

		Initialized = false;
		Running = false;
		Finalizing = false;
		isHaveFocus = true;

		doExitOnEscape = true;
		doLimitFPS = false;
		doCalcFPS = true;

		FPSText = NULL;
		RootGameObject = NULL;
	}

}	//	namespace Deku2D
