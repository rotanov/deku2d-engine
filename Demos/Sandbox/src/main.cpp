#include "2de_Engine.h"
#include "Game.h"
#include "2de_MathUtils.h"
//#include "LuaUtils.h"

bool Init()
{	
// 	Vector2 V(1.0f, 1.0f), U(2.0f, 2.0f), W(3.0f, 3.0f);
// 	(V += U) += W;
// 	W  = V / 0.0f;
	CTestUnit *Unit = new CTestUnit;
	return true;
}

bool Draw()
{	
	unsigned long FPS = 0;
	CEngine::Instance()->GetState(CEngine::STATE_FPS_COUNT, &FPS);
	CFontManager::Instance()->Print(0.0f, 0.0f, 0.0f, itos(FPS));
	return true;
}


int	main(int argc, char *argv[])
{
	CEngine* Ninja = CEngine::Instance();
	Ninja->SetState(CEngine::STATE_CONFIG_PATH, "Config/");
	Ninja->SetState(CEngine::STATE_CONFIG_NAME, "Sandbox.xml");
	Ninja->SetState(CEngine::STATE_USER_INIT_FUNC, &Init);
	Ninja->SetState(CEngine::STATE_RENDER_FUNC, &Draw);	
	Ninja->Run();
	return 0;
}
