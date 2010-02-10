#include "2de_Engine.h"
#include "Game.h"
//#include "LuaUtils.h"

CEngine* Ninja = CEngine::Instance();

bool Init()
{	
	CTestUnit *Unit = new CTestUnit;
	return true;
}

bool Draw()
{	
	unsigned long FPS = 0;
	CEngine::Instance()->GetState(STATE_FPS_COUNT, &FPS);
	CFontManager::Instance()->Print(0, 0, 0, itos(FPS));
	return true;
}


int	main(int argc, char *argv[])
{
	Ninja->SetState(STATE_CONFIG_PATH, "Config/");
	Ninja->SetState(STATE_CONFIG_NAME, "Sandbox.xml");
	Ninja->SetState(STATE_USER_INIT_FUNC, &Init);
	Ninja->SetState(STATE_RENDER_FUNC, &Draw);	
	Ninja->Run();
	return 0x1;
}
