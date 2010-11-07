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

class CSomeUpdatable : public CObject
{
public:
	void ProcessEvent(const CEvent &AEvent)
	{
		if (AEvent.GetName() == "EveryFrame")
		{
			CRenderableComponent *MagicCircle = CFactory::Instance()->Get<CRenderableComponent>("Magic circle");
			MagicCircle->SetAngle(MagicCircle->GetAngle() + 1000.0f * CEngine::Instance()->GetDeltaTime());
			CRenderableComponent *temptext = CFactory::Instance()->Get<CRenderableComponent>("Mouse cursor");
			temptext->SetAngle(temptext->GetAngle() + 100.0f * CEngine::Instance()->GetDeltaTime());
			CRenderableComponent *onemore = CFactory::Instance()->Get<CRenderableComponent>("Magic square");
			onemore->SetAngle(temptext->GetAngle() + 100.0f * CEngine::Instance()->GetDeltaTime());
		}
	}
};

bool CCustomStateHandler::OnInitialize()
{

	CEngine::Instance()->ToggleShowFPS(true);

	CAbstractScene *NewScene = CSceneManager::Instance()->CreateScene();
	CSceneManager::Instance()->SetCurrentScene(NewScene);

	// Lua scripting test
	//CLuaVirtualMachine::Instance()->RunFile(CResourceManager::Instance()->DataPath + "/Scripts/test.lua");
	//CLuaVirtualMachine::Instance()->RunScript(CFactory::Instance()->Get<CScript>("test"));
	//CLuaVirtualMachine::Instance()->RunScript(CFactory::Instance()->Get<CScript>("scriptable-test"));

	CScriptableComponent *sc = new CScriptableComponent(CFactory::Instance()->Get<CScript>("scriptable-test"));
	CFactory::Instance()->Add(sc, "ScriptableComponent");
	CUpdateManager::Instance()->RootGameObject->Attach(sc);

	// Some boxes
/*
	for(unsigned int i = 0; i < 10; i++)
		for(unsigned int j = 0; j < 10; j++)
		{
			CTestGameObject *GrBox = new CTestGameObject;
			GrBox->SetGrBox(Vector2(-10.0f, -10.0f), Vector2(10.0f, 10.0f));
			GrBox->Position = Vector2(170.0f + i * 32.0f, 120.0f + j * 24.0f);
		}*/

 //CTestGameObject *TestObject = new CTestGameObject();
	CRenderableComponent *NewRenderableComponent = CFactory::Instance()->New<CRenderableComponent>("Magic circle");
 
 NewRenderableComponent->SetModel(CRenderManager::CreateModelCircle(20.0f, MODEL_TYPE_LINES, 4));


 CRenderableComponent *magic_square = CFactory::Instance()->New<CRenderableComponent>("Magic square");

 magic_square->SetModel(CRenderManager::CreateModelCircle(10.0f, MODEL_TYPE_LINES, 16));
 magic_square->SetPosition(Vector2(50.0f, 50.0f));

 NewRenderableComponent->Attach(magic_square);

 
// CRenderableComponent *NewNewRenderableComponent = new CRenderableComponent(/*new CModelLine(Vector2(0.0f, 0.0f), Vector2(-64.0f, 64.0f))*/);
// 
// Vector2Array<4> vert;
// Vector2Array<4> tcor;
// 
// vert[0] = Vector2(0.0f, -64.0f);
// vert[1] = Vector2(64.0f, -64.0f);
// vert[2] = Vector2(64.0f, 0.0f);
// vert[3] = Vector2(0.0f, 0.0f);
// 
// tcor = V2_QUAD_BIN;
// 
// //CModelQuad * NewQuadModel = new CModelQuad(vert, tcor, CTextureManager::Instance()->Get("PacmanTiles"));
// 
// CRenderableComponent *NewNewNewRenderableComponent = new CRenderableComponent(/*NewQuadModel*/);
// 
// NewNewNewRenderableComponent->Configuration.Color = RGBAf(0.9f, 0.6f, 0.5f, 1.0f);
// 
// NewRenderableComponent->Configuration.Position = Vector2(128.0f, 128.0f);
 //TestObject->Attach(NewRenderableComponent);
 NewRenderableComponent->SetPosition(Vector2(100.0f, 100.0f));
// NewRenderableComponent->Attach(NewNewRenderableComponent);
// NewNewRenderableComponent->Attach(NewNewNewRenderableComponent);
 //CUpdateManager::Instance()->RootGameObject->Attach(TestObject);
 CRenderableComponent *temptext = CFactory::Instance()->Get<CRenderableComponent>("Mouse cursor");
 temptext->Attach(NewRenderableComponent);

 CSomeUpdatable *SomeUpdatable = new CSomeUpdatable();//CFactory::Instance()->New<CSomeUpdatable>("some updatable");
// Mouse cursor
 //sc->Attach(NewRenderableComponent);
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
