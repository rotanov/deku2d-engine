#include "2de_Engine.h"

#define ENGINE CEngine::Instance()
#define RENDER CRenderManager::Instance()
CEngine* Ninja = ENGINE;

class CTest : public CGameObject
{
public:
	CTest()
	{
		CEventManager::Instance()->Subscribe("KeyDown", this);
	}

	void ProcessEvent(const CEvent &AEvent)
	{
		if (AEvent.GetName() == "KeyDown" && AEvent.GetData<Uint16>("Sym") == SDLK_SPACE)
		{
			if (CGLWindow::Instance()->GetFullscreen())
			{
				CGLWindow::Instance()->SetFullscreen(false);
				CGLWindow::Instance()->SetSize(640, 480);
			}
			else
			{
				CGLWindow::Instance()->SetSize(1920, 1080);
				CGLWindow::Instance()->SetFullscreen(true);
			}
			CGLWindow::Instance()->Initialize();
		}
	}

};

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
	CUpdateManager::Instance()->RootGameObject->Attach(CFactory::Instance()->New<CTest>("SetSizeTest"));
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
