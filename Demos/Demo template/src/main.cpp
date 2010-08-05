#include "2de_Engine.h"

CEngine	*Ninja = CEngine::Instance();

bool Init()
{	
	return true;
}

bool Draw()
{
	return true;
}

bool Update(float dt)
{
	return true;
}

int main(int argc, char *argv[])
{
	Ninja->SetState(CEngine::STATE_CONFIG_PATH, "Config/");
	Ninja->SetState(CEngine::STATE_CONFIG_NAME, "Demo template.xml");
	Ninja->SetState(CEngine::STATE_USER_INIT_FUNC, &Init);
	Ninja->SetState(CEngine::STATE_UPDATE_FUNC, &Update);
	Ninja->SetState(CEngine::STATE_RENDER_FUNC, &Draw);
	Ninja->Run();
	return 0;
}
