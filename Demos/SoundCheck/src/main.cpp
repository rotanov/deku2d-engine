#include "2de_Engine.h"
#include "SoundCheck.h"

CEngine *Engine = CEngine::Instance();

CSoundCheck *SoundCheck = NULL;

bool Init()
{
	SoundCheck = CFactory::Instance()->New<CSoundCheck>("SoundCheck");
	return true;
}

int main(int argc, char* argv[])
{
	Engine->SetState(CEngine::STATE_USER_INIT_FUNC, &Init);
	Engine->SetState(CEngine::STATE_CONFIG_PATH, "Config/");
	Engine->SetState(CEngine::STATE_CONFIG_NAME, "SoundCheck.xml");
	Engine->Run();

	return 0;
}
