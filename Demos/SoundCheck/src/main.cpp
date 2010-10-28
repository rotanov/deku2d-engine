#include "2de_Engine.h"
#include "SoundCheck.h"

CEngine *Engine = CEngine::Instance();

CSoundCheck *SoundCheck = NULL;

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	bool OnInitialize();

};

bool CCustomStateHandler::OnInitialize()
{
	CAbstractScene *MainScene = CSceneManager::Instance()->CreateScene();
	CSceneManager::Instance()->SetCurrentScene(MainScene);

	SoundCheck = CFactory::Instance()->New<CSoundCheck>("SoundCheck");
	return true;
}

int main(int argc, char* argv[])
{
	Engine->SetStateHandler<CCustomStateHandler>();
	Engine->SetProgramName("SoundCheck");
	Engine->Run(argc, argv);

	return 0;
}
