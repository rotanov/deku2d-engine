#include "2de_Engine.h"
#include "NinjaStory.h"

CEngine	*Ninja	=	CEngine::Instance();

bool Init()
{	
	CNinjaStoryGame *NinjaStoryGame = new CNinjaStoryGame();
	return true;
}


int	main(int argc, char *argv[])
{
	Ninja->SetState(CEngine::STATE_CONFIG_PATH, "Config/");
	Ninja->SetState(CEngine::STATE_CONFIG_NAME, "Ninja story.xml");
	Ninja->SetState(CEngine::STATE_USER_INIT_FUNC, &Init);
	Ninja->Run();
	return 0x00;
}
