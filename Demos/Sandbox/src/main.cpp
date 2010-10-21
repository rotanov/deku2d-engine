#include "2de_Engine.h"
#include "Game.h"

#define ENGINE CEngine::Instance()
#define RENDER CRenderManager::Instance()
CEngine* Ninja = ENGINE;

class CGrBox : public CRenderable, public CUpdatable
{
private:
	CBox Box;

public:
	CGrBox()
	{
		Position = Vector2(320.0f, 240.0f);
		SetBox(CGrBox::Box);
	}

	void SetGrBox(const Vector2 &Min, const Vector2 &Max)
	{
		Box = CBox(Min, Max);
		SetBox(Box);
	}

	void Render()
	{
		CRenderManager::Instance()->DrawSolidBox(this, Box);
	}

	void Update(float dt)
	{
		Position += Vector2::Random(-0.1f, 0.1f);
	}
};

/*class */

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	bool OnInitialize();

};

bool CCustomStateHandler::OnInitialize()
{

	CEngine::Instance()->ToggleShowFPS(false);

	// создать сцену, добавить в неё *нихера*, сделать её текущей
	CAbstractScene *NewScene = CSceneManager::Instance()->CreateScene();
	CSceneManager::Instance()->SetCurrentScene(NewScene);

	// Lua scripting test
//	CLuaVirtualMachine::Instance()->RunFile(CResourceManager::Instance()->DataPath + "/Scripts/test.lua");

	// Some boxes
	for(unsigned int i = 0; i < 10; i++)
		for(unsigned int j = 0; j < 10; j++)
		{
			CGrBox *GrBox = new CGrBox;
			GrBox->SetGrBox(Vector2(-10.0f, -10.0f), Vector2(10.0f, 10.0f));
			GrBox->Position = Vector2(170.0f + i * 32.0f, 120.0f + j * 24.0f);
		}

	return true;
}

int main(int argc, char *argv[])
{
	Ninja->SetProgramName("Sandbox");
	Ninja->SetStateHandler<CCustomStateHandler>();
	Ninja->Run(argc, argv);
	return 0;
}
