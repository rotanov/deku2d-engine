#include "2de_Engine.h"
#include "NinjaStory.h"
using namespace Deku2d;

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
		EventManager->Subscribe("KeyDown", this);
		EventManager->Subscribe("KeyUp", this);
	}
	void ProcessEvent(const CEvent &AEvent)
	{
		if (AEvent.GetName() == "KeyDown")
		{
			Uint16 key = AEvent.GetData<Uint16>("Sym");
			switch (key)
			{
			case SDLK_z:
				EventManager->TriggerEvent("Jump", this);
				break;
			case SDLK_LEFT:
				EventManager->TriggerEvent("PlayerMoveLeft", this);
				break;
			case SDLK_RIGHT:
				EventManager->TriggerEvent("PlayerMoveRight", this);
				break;
			case SDLK_UP:
				EventManager->TriggerEvent("PlayerMoveUp", this);
				break;
			case SDLK_DOWN:
				EventManager->TriggerEvent("PlayerMoveDown", this);
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
				EventManager->TriggerEvent("PlayerOneStop", this);
				break;
			case SDLK_UP:
			case SDLK_DOWN:
				EventManager->TriggerEvent("PlayerTwoStop", this);
				break;
			}
		}
	}

};


bool CCustomStateHandler::OnInitialize()
{

	CTempKeyBindMan *tkbm = Factory->New<CTempKeyBindMan>("TempKeyBindMan");
	//CNinjaStoryGame *NinjaStoryGame = new CNinjaStoryGame();
// 	CAbstractScene *SomeNewScene = SceneManager->CreateScene();
// 	SceneManager->SetCurrentScene(SomeNewScene);

	CRenderableComponent *PlayerR = new CRenderableComponent(CRenderManager::CreateModelBox(32, 64));

	CGameObject *PlayerScriptable = Factory->New<CGameObject>("PlayerOneScriptable");
	
	PlayerScriptable->SetScript(Factory->Get<CScript>("Player"));

//	CScriptableComponent *PongGame = new CScriptableComponent(Factory->Get<CScript>("PongGameScript"));

	Factory->Add(PlayerR, "PlayerOne");
	//Factory->Add(PlayerScriptable, "PlayerOneScriptable");
//	Factory->Add(PongGame, "PongGame");

	LuaVirtualMachine->RunScript(Factory->Get<CScript>(Config->Section("Data")["InitScript"]));


	return true;
}

int main(int argc, char *argv[])
{
	Ninja->SetProgramName("Ninja story");
	Ninja->SetStateHandler<CCustomStateHandler>();
	Ninja->Run(argc, argv);
	return EXIT_SUCCESS;
}
