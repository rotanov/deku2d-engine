#include "2de_Engine.h"
#include "Pong.h"


CPongGame *PongGame = NULL; 

class CChangeSceneToPackMan : public CAbstractAction
{
public:
	CAbstractScene *PongScn;
	virtual void Execute()
	{
		CSceneManager::Instance()->SetCurrentScene(PongScn);
	}	
};

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	void OnInitialize();

};

void CCustomStateHandler::OnInitialize()
{
	CAbstractScene *PongScene = CSceneManager::Instance()->CreateScene();
	CAbstractScene *SomeScene = CSceneManager::Instance()->GetCurrentScene();
	CSceneManager::Instance()->SetCurrentScene(PongScene);
	PongGame = CFactory::Instance()->New<CPongGame>("PongGame");
	CSceneManager::Instance()->SetCurrentScene(SomeScene);


	CTimeredAction<CChangeSceneToPackMan> *Action = new CTimeredAction<CChangeSceneToPackMan>();
	Action->Action.PongScn = PongScene;
	Action->SetLife(5.0f);
}

int main(int argc, char* argv[])
{
	CEngine *Ninja = CEngine::Instance();
	Ninja->SetStateHandler<CCustomStateHandler>();
	CEnvironment::Paths::SetConfigPath("Config/");
	Ninja->SetState(CEngine::STATE_CONFIG_NAME, "Pong.xml");
	Ninja->Run();
	return 0;
}
