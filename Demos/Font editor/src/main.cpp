#include "Font editor.h"

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	bool OnInitialize();

};

bool CCustomStateHandler::OnInitialize()
{
	CFontEditor *FontEditor = CFactory::Instance()->New<CFontEditor>("FontEditor");
	CEngine::Instance()->ToggleKeyRepeat(true); // temporarily for GUI-only programs..
	return true;
}

int main(int argc, char *argv[])
{
	CEngine	*Ninja = CEngine::Instance();
	Ninja->SetProgramName("Font editor");
	Ninja->SetStateHandler<CCustomStateHandler>();
	Ninja->Run();
	return 0;
}
