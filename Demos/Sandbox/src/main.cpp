#include "2de_Engine.h"
#include "Game.h"

#define ENGINE CEngine::Instance()
#define RENDER CRenderManager::Instance()
CEngine* Ninja = ENGINE;

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	void OnInitialize();

};

void CCustomStateHandler::OnInitialize()
{
	// создать сцену, добавить в неё *нихера*, сделать её текущей
	CAbstractScene *NewScene = CSceneManager::Instance()->CreateScene();
	CSceneManager::Instance()->SetCurrentScene(NewScene);
}

bool Draw()
{
	
	return true;
}

bool Update(float dt)
{
	return true;
}


int main(int argc, char *argv[])
{
	CEnvironment::Paths::SetConfigPath("Config/");
	Ninja->SetState(CEngine::STATE_CONFIG_NAME, "Sandbox.xml");
	Ninja->SetStateHandler<CCustomStateHandler>();
	Ninja->SetState(CEngine::STATE_UPDATE_FUNC, &Update);
	Ninja->SetState(CEngine::STATE_RENDER_FUNC, &Draw);
	Ninja->Run();
	return 0;
}
