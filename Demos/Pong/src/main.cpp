#include "2de_Engine.h"

using namespace Deku2D;

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
