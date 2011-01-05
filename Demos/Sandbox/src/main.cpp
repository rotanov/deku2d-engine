#include "2de_Engine.h"

#define ENGINE CEngine::Instance()
#define RENDER CRenderManager::Instance()
CEngine* Ninja = ENGINE;


class CCustomStateHandler : public CAbstractStateHandler
{
public:
	bool OnInitialize();
	void OnFinalize();
};

bool CCustomStateHandler::OnInitialize()
{
	CAbstractScene *NewScene = CSceneManager::Instance()->CreateScene();
	CSceneManager::Instance()->SetCurrentScene(NewScene);
	CLuaVirtualMachine::Instance()->RunScript(CFactory::Instance()->Get<CScript>(CConfig::Instance()->Section("Data")["InitScript"]));
	return true;
}

void CCustomStateHandler::OnFinalize()
{
	// wait...what?
}

int main(int argc, char *argv[])
{
	Ninja->SetProgramName("Sandbox");
	Ninja->SetStateHandler<CCustomStateHandler>();
	Ninja->Run(argc, argv);
	return 0;
}
