#include "2de_Engine.h"
#include "Game.h"

#define ENGINE CEngine::Instance()
#define RENDER CRenderManager::Instance()
CEngine* Ninja = ENGINE;


bool Init()
{	
	// создать сцену, добавить в неё *нихера*, сделать её текущей
	CAbstractScene *NewScene = CSceneManager::Instance()->CreateScene();
	CSceneManager::Instance()->SetCurrentScene(NewScene);
	return true;
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
	Ninja->SetState(CEngine::STATE_CONFIG_PATH, "Config/");
	Ninja->SetState(CEngine::STATE_CONFIG_NAME, "Sandbox.xml");
	Ninja->SetState(CEngine::STATE_USER_INIT_FUNC, &Init);
	Ninja->SetState(CEngine::STATE_UPDATE_FUNC, &Update);
	Ninja->SetState(CEngine::STATE_RENDER_FUNC, &Draw);
	Ninja->Run();
	return 0;
}
