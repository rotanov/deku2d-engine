#include "2de_Engine.h"
#include "Pong.h"


CPongGame *PongGame = NULL; 

bool Init()
{
	PongGame = new CPongGame();
	return true;
}

int main(int argc, char* argv[])
{
	CEngine *Ninja = CEngine::Instance();
	Ninja->SetState(CEngine::STATE_USER_INIT_FUNC, &Init);
	Ninja->SetState(CEngine::STATE_CONFIG_PATH, "Config/");
	Ninja->SetState(CEngine::STATE_CONFIG_NAME, "Pong.xml");
	Ninja->Run();
	return 0;
}
