#include "2de_Engine.h"
#include "SoundCheck.h"

CEngine *Engine = CEngine::Instance();

CSoundCheck *SoundCheck = NULL;

bool Init()
{
	SoundCheck = new CSoundCheck();
	return true;
}

int main(int argc, char* argv[])
{
	Engine->SetState(STATE_USER_INIT_FUNC, &Init);
	Engine->SetState(STATE_CONFIG_NAME, "SoundCheck.xml");
	Engine->Run();

	return 0;
}