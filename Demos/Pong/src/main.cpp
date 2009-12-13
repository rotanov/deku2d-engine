#include "2de_Engine.h"
#include "Pong.h"

CEngine *Ninja = CEngine::Instance();
CPongGame *PongGame = NULL; 

bool Init()
{
	PongGame = new CPongGame();
	return true;
}

int main(int argc, char* argv[])
{
	Ninja->SetState(STATE_USER_INIT_FUNC, &Init);
	Ninja->SetState(STATE_CONFIG_NAME, "Pong.xml");
	Ninja->Run();
	return true;
}
