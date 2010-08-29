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
	CAbstractScene *SomeNewScene = CSceneManager::Instance()->CreateScene();
	CSceneManager::Instance()->SetCurrentScene(SomeNewScene);

	CTemporaryBouncingTextTest *BouncingTextTest = CFactory::Instance()->New<CTemporaryBouncingTextTest>("BouncingTextTest");

	return true;
}

int main(int argc, char *argv[])
{
	CEnvironment::Paths::SetConfigPath("Config/");
	Ninja->SetProgramName("Ninja story");
	Ninja->SetStateHandler<CCustomStateHandler>();
	Ninja->Run();
	return EXIT_SUCCESS;
}
