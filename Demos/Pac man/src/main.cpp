#include "2de_Engine.h"
#include "2de_GameUtils.h"
#include "Pacman.h"

CEngine	*Ninja	=	CEngine::Instance();

// bool Init()
// {	
// 	glEnable(GL_DEPTH_TEST);
// 	glDepthFunc(GL_LEQUAL);
// // 	new CPakmanGame(new CPacmanPlayer);
// // 	CSoundMixer::Instance()->PlayMusic(CMusicManager::Instance()->GetMusicByName("PacMan"), 10000);
// 	return true;
// }

int	main(int argc, char *argv[])
{
	Ninja->SetState(CEngine::STATE_CONFIG_PATH, "Config/");
	Ninja->SetState(CEngine::STATE_CONFIG_NAME, "Pac man.xml");
	//Ninja->SetState(CEngine::STATE_USER_INIT_FUNC, &Init);
	Ninja->Run();
	return 0x00;
}
