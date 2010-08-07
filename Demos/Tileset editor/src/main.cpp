#include "2de_Engine.h"
#include "Tileset editor.h"

CEngine	*Ninja = CEngine::Instance();

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	void OnInitialize();

};

void CCustomStateHandler::OnInitialize()
{
	CTilesetEditor *TilesetEditor = CFactory::Instance()->New<CTilesetEditor>("Tileset editor");
}

int main(int argc, char *argv[])
{
	CEnvironment::Paths::SetConfigPath("Config/");
	Ninja->SetState(CEngine::STATE_CONFIG_NAME, "Tileset editor.xml");
	Ninja->SetStateHandler<CCustomStateHandler>();
	Ninja->Run();
	return 0;
}
