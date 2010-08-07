#include "2de_Engine.h"
#include "NinjaStory.h"

CEngine *Ninja = CEngine::Instance();

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	void OnInitialize();

};

void CCustomStateHandler::OnInitialize()
{
	//CNinjaStoryGame *NinjaStoryGame = new CNinjaStoryGame();
	CAbstractScene *SomeNewScene = CSceneManager::Instance()->CreateScene();
	CSceneManager::Instance()->SetCurrentScene(SomeNewScene);
}

bool Update(float dt)
{
	static float age = 1.0f;
	age += dt;
	if (Ninja->keys[SDLK_SPACE] && age >= 0.001f)
	{
		CBouncingText *newbt = CFactory::Instance()->New<CBouncingText>(""); //new CBouncingText(); 
		newbt->SetText(itos(Random_Int(1, 999)) + "dmg");
		age = 0.0f;
	}
	return true;
}


int main(int argc, char *argv[])
{
	CEnvironment::Paths::SetConfigPath("Config/");
	Ninja->SetState(CEngine::STATE_CONFIG_NAME, "Ninja story.xml");
	Ninja->SetStateHandler<CCustomStateHandler>();
	Ninja->SetState(CEngine::STATE_UPDATE_FUNC, &Update);
	Ninja->Run();
	return EXIT_SUCCESS;
}
