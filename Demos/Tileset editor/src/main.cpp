#include "2de_Engine.h"
#include "Tileset editor.h"

CEngine	*Ninja = CEngine::Instance();

bool Init()
{	
	CTilesetEditor *TilesetEditor = CFactory::Instance()->New<CTilesetEditor>("Tileset editor");
	return true;
}


int main(int argc, char *argv[])
{
	Ninja->SetState(CEngine::STATE_CONFIG_PATH, "Config/");
	Ninja->SetState(CEngine::STATE_CONFIG_NAME, "Tileset editor.xml");
	Ninja->SetState(CEngine::STATE_USER_INIT_FUNC, &Init);
	Ninja->Run();
	return 0;
}
