#include "2de_Engine.h"
#include "Pong.h"

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

	//CSceneManager::Instance()->SetCurrentScene(SomeScene);

	CTempKeyBindMan *tkbm = CFactory::Instance()->New<CTempKeyBindMan>("TempKeyBindMan");

	// TODO:
	// 	1. precompile built-in Lua scripts
	// 	2. automatically load them

	CLuaVirtualMachine::Instance()->RunScript(CFactory::Instance()->Get<CScript>("BUILTIN_Vector2"));
	CLuaVirtualMachine::Instance()->RunScript(CFactory::Instance()->Get<CScript>("BUILTIN_CBox"));

	CRenderableComponent *PlayerOne = new CRenderableComponent(CRenderManager::CreateModelBox(PONG_PLAYER_WIDTH, PONG_PLAYER_HEIGHT));
	CRenderableComponent *PlayerTwo = new CRenderableComponent(CRenderManager::CreateModelBox(PONG_PLAYER_WIDTH, PONG_PLAYER_HEIGHT));
	CRenderableComponent *Ball = new CRenderableComponent(CRenderManager::CreateModelBox(PONG_BALL_SIZE, PONG_BALL_SIZE));

	CTimerComponent *MemoryUsageUpdateTimer = new CTimerComponent(3);
	MemoryUsageUpdateTimer->SetEnabled(true);

	CFactory::Instance()->Add(MemoryUsageUpdateTimer, "MemoryUsageUpdateTimer");
	CFactory::Instance()->Add(PlayerOne, "PlayerOne");
	CFactory::Instance()->Add(PlayerTwo, "PlayerTwo");
	CFactory::Instance()->Add(Ball, "Ball");

	CLuaVirtualMachine::Instance()->RunScript(CFactory::Instance()->Get<CScript>(CConfig::Instance()->Section("Data")["InitScript"]));

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
