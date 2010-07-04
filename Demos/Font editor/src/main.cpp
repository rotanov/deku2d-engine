#include "Font editor.h"

bool Init()
{	
	CFontEditor *FontEditor = CFactory::Instance()->New<CFontEditor>("FontEditor");
	return true;
}


int main(int argc, char *argv[])
{
	CEngine	*Ninja = CEngine::Instance();
	Ninja->SetState(CEngine::STATE_CONFIG_PATH, "Config/");
	Ninja->SetState(CEngine::STATE_CONFIG_NAME, "FontEditor.xml");
	Ninja->SetState(CEngine::STATE_USER_INIT_FUNC, &Init);
	Ninja->Run();
	return 0;
}
