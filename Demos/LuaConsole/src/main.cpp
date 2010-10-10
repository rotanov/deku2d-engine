#include "2de_Engine.h"

#include "LuaConsole.h"

CEngine *Engine = CEngine::Instance();

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	bool OnInitialize();
};

bool CCustomStateHandler::OnInitialize()
{
	CAbstractScene *MainScene = CSceneManager::Instance()->CreateScene();
	CSceneManager::Instance()->SetCurrentScene(MainScene);

	CLuaConsole *LuaConsole = CFactory::Instance()->New<CLuaConsole>("Lua Console");

	return true;
}

int main(int argc, char *argv[])
{
	Engine->SetProgramName("LuaConsole");
	Engine->SetStateHandler<CCustomStateHandler>();
	Engine->Run(argc, argv);
	return 0;
}



