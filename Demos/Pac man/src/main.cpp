#include "2de_Engine.h"
#include "2de_GameUtils.h"
#include "Pacman.h"

CEngine	*Ninja	=	CEngine::Instance();

bool Init()
{	
	CPakmanGame *PacManGame = (new CPakmanGame);
	PacManGame->SetPlayer(new CPacmanPlayer);
	return true;
}

int	main(int argc, char *argv[])
{
	Ninja->SetState(CEngine::STATE_CONFIG_PATH, "Config/");
	Ninja->SetState(CEngine::STATE_CONFIG_NAME, "Pac man.xml");
	Ninja->SetState(CEngine::STATE_USER_INIT_FUNC, &Init);
	Ninja->Run();
	return 0x00;
}
