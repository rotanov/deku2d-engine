#include "2de_Engine.h"
#include "Game.h"

#define ENGINE CEngine::Instance()
#define RENDER CRenderManager::Instance()
CEngine* Ninja = ENGINE;

class CTestGameObject : public CGameObject
{

};

/*class */

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	bool OnInitialize();
	void OnFinalize();

};

bool CCustomStateHandler::OnInitialize()
{

	CEngine::Instance()->ToggleShowFPS(true);

	// создать сцену, добавить в неё *нихера*, сделать её текущей
	CAbstractScene *NewScene = CSceneManager::Instance()->CreateScene();
	CSceneManager::Instance()->SetCurrentScene(NewScene);

	// Lua scripting test
//	CLuaVirtualMachine::Instance()->RunFile(CResourceManager::Instance()->DataPath + "/Scripts/test.lua");

	// Some boxes
/*
	for(unsigned int i = 0; i < 10; i++)
		for(unsigned int j = 0; j < 10; j++)
		{
			CTestGameObject *GrBox = new CTestGameObject;
			GrBox->SetGrBox(Vector2(-10.0f, -10.0f), Vector2(10.0f, 10.0f));
			GrBox->Position = Vector2(170.0f + i * 32.0f, 120.0f + j * 24.0f);
		}*/

CTestGameObject *TestObject = new CTestGameObject();
CRenderableComponent *NewRenderableComponent = new CRenderableComponent(new CModelLine(Vector2(0.0f, 0.0f), Vector2(128.0f, 128.0f)));
CRenderableComponent *NewNewRenderableComponent = new CRenderableComponent(new CModelLine(Vector2(0.0f, 0.0f), Vector2(-64.0f, 64.0f)));

NewRenderableComponent->Configuration.Position = Vector2(128.0f, 128.0f);
TestObject->Attach(NewRenderableComponent);
NewRenderableComponent->Attach(NewNewRenderableComponent);
CUpdateManager::Instance()->RootGameObject.Attach(TestObject);
//TestObject->AttachChild(new CUpdatable());

/*
Position = Vector2(320.0f, 240.0f);
SetBox(CTestGameObject::Box);
// Опять же скрипт OnEveryFrame хотя можно и в сущность CUpdatable
Position += Vector2::Random(-0.1f, 0.1f);
*/

	return true;
}

void CCustomStateHandler::OnFinalize()
{

}

int main(int argc, char *argv[])
{
	Ninja->SetProgramName("Sandbox");
	Ninja->SetStateHandler<CCustomStateHandler>();
	Ninja->Run(argc, argv);
	return 0;
}
