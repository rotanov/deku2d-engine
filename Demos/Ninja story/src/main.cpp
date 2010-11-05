#include "2de_Engine.h"
#include "NinjaStory.h"

CEngine *Ninja = CEngine::Instance();

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	bool OnInitialize();

};

bool CCustomStateHandler::OnInitialize()
{
	//CNinjaStoryGame *NinjaStoryGame = new CNinjaStoryGame();
	//CAbstractScene *SomeNewScene = CSceneManager::Instance()->CreateScene();
	//CSceneManager::Instance()->SetCurrentScene(SomeNewScene);

	return true;
}

int main(int argc, char *argv[])
{
	Ninja->SetProgramName("Ninja story");
	Ninja->SetStateHandler<CCustomStateHandler>();
	Ninja->Run(argc, argv);
	return EXIT_SUCCESS;
}
