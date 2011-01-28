#include "2de_Engine.h"
#include "NinjaStory.h"

CEngine *Ninja = CEngine::Instance();

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	bool OnInitialize();

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
			case SDLK_z:
				CEventManager::Instance()->TriggerEvent("Jump", this);
				break;
			case SDLK_LEFT:
				CEventManager::Instance()->TriggerEvent("PlayerMoveLeft", this);
				break;
			case SDLK_RIGHT:
				CEventManager::Instance()->TriggerEvent("PlayerMoveRight", this);
				break;
			case SDLK_UP:
				CEventManager::Instance()->TriggerEvent("PlayerMoveUp", this);
				break;
			case SDLK_DOWN:
				CEventManager::Instance()->TriggerEvent("PlayerMoveDown", this);
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


bool CCustomStateHandler::OnInitialize()
{

	CTempKeyBindMan *tkbm = CFactory::Instance()->New<CTempKeyBindMan>("TempKeyBindMan");
	//CNinjaStoryGame *NinjaStoryGame = new CNinjaStoryGame();
// 	CAbstractScene *SomeNewScene = CSceneManager::Instance()->CreateScene();
// 	CSceneManager::Instance()->SetCurrentScene(SomeNewScene);

	CLuaVirtualMachine::Instance()->RunScript(CFactory::Instance()->Get<CScript>("BUILTIN_Vector2"));
	CLuaVirtualMachine::Instance()->RunScript(CFactory::Instance()->Get<CScript>("BUILTIN_CBox"));

	CRenderableComponent *PlayerR = new CRenderableComponent(CRenderManager::CreateModelBox(32, 64));

	CGameObject *PlayerScriptable = CFactory::Instance()->New<CGameObject>("PlayerOneScriptable");
	
	PlayerScriptable->SetScript(CFactory::Instance()->Get<CScript>("Player"));

//	CScriptableComponent *PongGame = new CScriptableComponent(CFactory::Instance()->Get<CScript>("PongGameScript"));

	CFactory::Instance()->Add(PlayerR, "PlayerOne");
	//CFactory::Instance()->Add(PlayerScriptable, "PlayerOneScriptable");
//	CFactory::Instance()->Add(PongGame, "PongGame");

	CLuaVirtualMachine::Instance()->RunScript(CFactory::Instance()->Get<CScript>(CConfig::Instance()->Section("Data")["InitScript"]));


	return true;
}

int main(int argc, char *argv[])
{
	Ninja->SetProgramName("Ninja story");
	Ninja->SetStateHandler<CCustomStateHandler>();
	Ninja->Run(argc, argv);
	return EXIT_SUCCESS;
}
