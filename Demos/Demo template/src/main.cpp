#include "2de_Engine.h"

CEngine	*Ninja = CEngine::Instance();

bool Init()
{	
	int *some = new int;
	return true;
}


int main(int argc, char *argv[])
{
	Ninja->SetState(CEngine::STATE_CONFIG_PATH, "Config/");
	Ninja->SetState(CEngine::STATE_CONFIG_NAME, "Demo template.xml");
	Ninja->SetState(CEngine::STATE_USER_INIT_FUNC, &Init);
	Ninja->Run();
	return 0;
}
