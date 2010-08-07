#include "Font editor.h"

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	void OnInitialize();

};

void CCustomStateHandler::OnInitialize()
{
	CFontEditor *FontEditor = CFactory::Instance()->New<CFontEditor>("FontEditor");
}

int main(int argc, char *argv[])
{
	CEngine	*Ninja = CEngine::Instance();
	CEnvironment::Paths::SetConfigPath("Config/");
	Ninja->SetState(CEngine::STATE_CONFIG_NAME, "FontEditor.xml");
	Ninja->SetStateHandler<CCustomStateHandler>();
	Ninja->Run();
	return 0;
}
