#include "2de_Engine.h"
#include "Pong.h"


CPongGame *PongGame = NULL;

class CChangeSceneToPackMan : public CAbstractAction
{
public:
	CAbstractScene *PongScn;
	virtual void Execute()
	{
		CSceneManager::Instance()->SetCurrentScene(PongScn);
	}	
};

class CTempKeyBindMan : public CObject
{
public:
	CTempKeyBindMan()
	{
		CEventManager::Instance()->Subscribe("KeyDown", this);
		CEventManager::Instance()->Subscribe("KeyUp", this);
	}
	void ProcessEvent(const CEvent &AEvent)
	{
		if (AEvent.GetName() == "KeyDown")
		{
			Uint16 key = AEvent.GetData<Uint16>("Sym");
			switch (key)
			{
			case SDLK_SPACE:
				CEventManager::Instance()->TriggerEvent("Reset", this);
				break;
			case SDLK_w:
				CEventManager::Instance()->TriggerEvent("PlayerOneMoveUp", this);
				break;
			case SDLK_s:
				CEventManager::Instance()->TriggerEvent("PlayerOneMoveDown", this);
				break;
			case SDLK_UP:
				CEventManager::Instance()->TriggerEvent("PlayerTwoMoveUp", this);
				break;
			case SDLK_DOWN:
				CEventManager::Instance()->TriggerEvent("PlayerTwoMoveDown", this);
				break;
			}

		}
		else if (AEvent.GetName() == "KeyUp")
		{
			Uint16 key = AEvent.GetData<Uint16>("Sym");
			switch (key)
			{
			case SDLK_w:
			case SDLK_s:
				CEventManager::Instance()->TriggerEvent("PlayerOneStop", this);
				break;
			case SDLK_UP:
			case SDLK_DOWN:
				CEventManager::Instance()->TriggerEvent("PlayerTwoStop", this);
				break;
			}
		}
	}

};

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	bool OnInitialize();

};

bool CCustomStateHandler::OnInitialize()
{
	CAbstractScene *PongScene = CSceneManager::Instance()->CreateScene();
	CAbstractScene *SomeScene = CSceneManager::Instance()->GetCurrentScene();
	CSceneManager::Instance()->SetCurrentScene(PongScene);
	//PongGame = CFactory::Instance()->New<CPongGame>("PongGame");
	//CSceneManager::Instance()->SetCurrentScene(SomeScene);


	//CTimeredAction<CChangeSceneToPackMan> *Action = CFactory::Instance()->New<CTimeredAction<CChangeSceneToPackMan> >("Change Scene Timered Action");
	//Action->Action.PongScn = PongScene;
	//Action->SetLife(1.0f);

	CTempKeyBindMan *tkbm = CFactory::Instance()->New<CTempKeyBindMan>("TempKeyBindMan");

	// TODO:
	// 	1. precompile built-in Lua scripts
	// 	2. automatically load them

	CLuaVirtualMachine::Instance()->RunScript(CFactory::Instance()->Get<CScript>("BUILTIN_Vector2"));
	CLuaVirtualMachine::Instance()->RunScript(CFactory::Instance()->Get<CScript>("BUILTIN_CBox"));

	//CLuaVirtualMachine::Instance()->RunScript(CFactory::Instance()->Get<CScript>("PongPlayers"));

	CRenderableComponent *PlayerOne = new CRenderableComponent(CRenderManager::CreateModelBox(PONG_PLAYER_WIDTH, PONG_PLAYER_HEIGHT));
	CRenderableComponent *PlayerTwo = new CRenderableComponent(CRenderManager::CreateModelBox(PONG_PLAYER_WIDTH, PONG_PLAYER_HEIGHT));
	CRenderableComponent *Ball = new CRenderableComponent(CRenderManager::CreateModelBox(PONG_BALL_SIZE, PONG_BALL_SIZE));

	CScriptableComponent *PlayerOneScriptable = new CScriptableComponent(CFactory::Instance()->Get<CScript>("PongPlayers"));
	CScriptableComponent *PlayerTwoScriptable = new CScriptableComponent(CFactory::Instance()->Get<CScript>("PongPlayers"));
	CScriptableComponent *BallScriptable = new CScriptableComponent(CFactory::Instance()->Get<CScript>("BallScript"));
	CScriptableComponent *PongGame = new CScriptableComponent(CFactory::Instance()->Get<CScript>("PongGameScript"));

	PlayerOne->Attach(PlayerOneScriptable);
	PlayerTwo->Attach(PlayerTwoScriptable);
	Ball->Attach(BallScriptable);

	CUpdateManager::Instance()->RootGameObject->Attach(PlayerOne);
	CUpdateManager::Instance()->RootGameObject->Attach(PlayerTwo);
	CUpdateManager::Instance()->RootGameObject->Attach(Ball);
	CUpdateManager::Instance()->RootGameObject->Attach(PongGame);

	// receiving Create event is useless on initialization... this is HACK:
	CFactory::Instance()->Add(PlayerOneScriptable, "PlayerOneScriptable");
	CFactory::Instance()->Add(PlayerTwoScriptable, "PlayerTwoScriptable");
	CFactory::Instance()->Add(BallScriptable, "BallScriptable");
	CFactory::Instance()->Add(PongGame, "PongGame");

	// when there will be runner, that constructs objects from XML, it should trigger Create event for all objects after they're attached to each other... or we should use something like OnAttach...

	return true;
}

int main(int argc, char* argv[])
{
	CEngine *Ninja = CEngine::Instance();
	Ninja->SetStateHandler<CCustomStateHandler>();
	Ninja->SetProgramName("Pong");
	Ninja->Run(argc, argv);
	return 0;
}
