#include "2de_Engine.h"
#include "Tileset editor.h"

CEngine	*Ninja = CEngine::Instance();

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	bool OnInitialize();

};

bool CCustomStateHandler::OnInitialize()
{
	CTilesetEditor *TilesetEditor = CFactory::Instance()->New<CTilesetEditor>("Tileset editor");
	CEngine::Instance()->ToggleKeyRepeat(true); // temporarily for GUI-only programs..
	return true;
}

int main(int argc, char *argv[])
{
	CEnvironment::Paths::SetConfigPath("Config/");
	Ninja->SetProgramName("Tileset editor");
	Ninja->SetStateHandler<CCustomStateHandler>();
	Ninja->Run();
	return 0;
}
