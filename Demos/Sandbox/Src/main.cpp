#include "2de_Engine.h"
#include "Game.h"
//#include "LuaUtils.h"

CEngine* Ninja = CEngine::Instance();

bool Init()
{	
	return true;
}

bool Draw()
{	
	return true;
}


int	main(int argc, char *argv[])
{
	Ninja->SetState(STATE_CONFIG_NAME, "Sandbox.xml");
	Ninja->SetState(STATE_USER_INIT_FUNC, &Init);
	Ninja->SetState(STATE_RENDER_FUNC, &Draw);	
	Ninja->Run();
	return 0x1;
}
