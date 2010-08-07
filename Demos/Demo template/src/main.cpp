#include "2de_Engine.h"

CEngine *Engine = CEngine::Instance();

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	void OnInitialize();
};

void CCustomStateHandler::OnInitialize()
{
	// do something..
	// i put it here just to show how it works and as placeholder for user's code..
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
	CEnvironment::Paths::SetConfigPath("Config/");
	Engine->SetState(CEngine::STATE_CONFIG_NAME, "Demo template.xml");
	Engine->SetStateHandler<CCustomStateHandler>();
	Engine->SetState(CEngine::STATE_UPDATE_FUNC, &Update);
	Engine->SetState(CEngine::STATE_RENDER_FUNC, &Draw);
	Engine->Run();
	return 0;
}
