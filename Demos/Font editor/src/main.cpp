#include "Font editor.h"

CEngine	*Ninja	=	CEngine::Instance();
CFontEditor *FontEditor = NULL;

bool Init()
{	
	FontEditor = new CFontEditor;
	return true;
}


int	main(int argc, char *argv[])
{
	Ninja->SetState(STATE_CONFIG_PATH, "Config/");
	Ninja->SetState(STATE_CONFIG_NAME, "FontEditor.xml");
	Ninja->SetState(STATE_USER_INIT_FUNC, &Init);
	Ninja->Run();
	return 0x00;
}
