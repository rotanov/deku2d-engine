#include "2de_Engine.h"
#include "Pong.h"
using namespace Deku2D;

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
			case SDLK_SPACE:
				EventManager->TriggerEvent("Reset", this);
				break;
			case SDLK_w:
				EventManager->TriggerEvent("PlayerOneMoveUp", this);
				break;
			case SDLK_s:
				EventManager->TriggerEvent("PlayerOneMoveDown", this);
				break;
			case SDLK_UP:
				EventManager->TriggerEvent("PlayerTwoMoveUp", this);
				break;
			case SDLK_DOWN:
				EventManager->TriggerEvent("PlayerTwoMoveDown", this);
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

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	bool OnInitialize();

};

bool CCustomStateHandler::OnInitialize()
{
	CAbstractScene *PongScene = SceneManager->CreateScene();
	CAbstractScene *SomeScene = SceneManager->GetCurrentScene();
	SceneManager->SetCurrentScene(PongScene);

	//SceneManager->SetCurrentScene(SomeScene);

	CTempKeyBindMan *tkbm = Factory->New<CTempKeyBindMan>("TempKeyBindMan");

	// TODO:
	// 	1. precompile built-in Lua scripts
	// 	2. automatically load them

	LuaVirtualMachine->RunScript(Factory->Get<CScript>(Config->Section("Data")["InitScript"]));

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
