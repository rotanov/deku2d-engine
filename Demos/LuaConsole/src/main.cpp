#include "2de_Engine.h"
using namespace Deku2d;

#include "LuaConsole.h"

CEngine *Engine = CEngine::Instance();

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	bool OnInitialize();
};

bool CCustomStateHandler::OnInitialize()
{
	CAbstractScene *MainScene = SceneManager->CreateScene();
	SceneManager->SetCurrentScene(MainScene);

	CLuaConsole *LuaConsole = Factory->New<CLuaConsole>("Lua Console");

	return true;
}

int main(int argc, char *argv[])
{
	Engine->SetProgramName("LuaConsole");
	Engine->SetStateHandler<CCustomStateHandler>();
	Engine->Run(argc, argv);
	return 0;
}



