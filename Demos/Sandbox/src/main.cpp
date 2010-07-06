#include "2de_Engine.h"
#include "Game.h"
#include "2de_MathUtils.h"
//#include "LuaUtils.h"

CText *FPSText = NULL;

bool Init()
{	
	CTestUnit *Unit = CFactory::Instance()->New<CTestUnit>("TestUnit");
	FPSText = CFactory::Instance()->New<CText>("FPSText");
	FPSText->Position = Vector2(0.0f, 0.0f);
	return true;
}

bool Update()
{	
	unsigned long FPS = 0;
	CEngine::Instance()->GetState(CEngine::STATE_FPS_COUNT, &FPS);
	FPSText->SetText("FPS: " + itos(FPS));
	return true;
}

int main(int argc, char *argv[])
{
	CEngine* Ninja = CEngine::Instance();
	Ninja->SetState(CEngine::STATE_CONFIG_PATH, "Config/");
	Ninja->SetState(CEngine::STATE_CONFIG_NAME, "Sandbox.xml");
	Ninja->SetState(CEngine::STATE_USER_INIT_FUNC, &Init);
	Ninja->SetState(CEngine::STATE_UPDATE_FUNC, &Update);	
	Ninja->Run();
	return 0;
}
