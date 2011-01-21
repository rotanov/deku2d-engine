#include "2de_Engine.h"

CEngine *Engine = CEngine::Instance();

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	bool OnInitialize();
};

bool CCustomStateHandler::OnInitialize()
{
	CAbstractScene *NewScene = CSceneManager::Instance()->CreateScene();
	CSceneManager::Instance()->SetCurrentScene(NewScene);

	CLuaVirtualMachine::Instance()->RunScript(CFactory::Instance()->Get<CScript>(CConfig::Instance()->Section("Data")["InitScript"]));
	return true;
}

int main(int argc, char *argv[])
{
	Engine->SetProgramName("Color lines");
	Engine->SetStateHandler<CCustomStateHandler>();
	Engine->Run(argc, argv);
	return EXIT_SUCCESS;
}



