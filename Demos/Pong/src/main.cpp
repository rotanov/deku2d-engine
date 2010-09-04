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
	bool OnInitialize();

};

bool CCustomStateHandler::OnInitialize()
{
	CAbstractScene *PongScene = CSceneManager::Instance()->CreateScene();
	CAbstractScene *SomeScene = CSceneManager::Instance()->GetCurrentScene();
	CSceneManager::Instance()->SetCurrentScene(PongScene);
	PongGame = CFactory::Instance()->New<CPongGame>("PongGame");
	CSceneManager::Instance()->SetCurrentScene(SomeScene);


	CTimeredAction<CChangeSceneToPackMan> *Action = CFactory::Instance()->New<CTimeredAction<CChangeSceneToPackMan> >("Change Scene Timered Action");
	Action->Action.PongScn = PongScene;
	Action->SetLife(1.0f);

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
