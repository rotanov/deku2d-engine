#include "2de_Engine.h"
#include "Game.h"

#define ENGINE CEngine::Instance()
#define RENDER CRenderManager::Instance()
CEngine* Ninja = ENGINE;


class CCustomStateHandler : public CAbstractStateHandler
{
public:
	bool OnInitialize();
	void OnFinalize();
};

class CSomeUpdatable : public CGameObject
{
public:
	CSomeUpdatable()
	{
		CEventManager::Instance()->Subscribe("EveryFrame", this);
	}
	void ProcessEvent(const CEvent &AEvent)
	{
		if (AEvent.GetName() == "EveryFrame")
		{
			CPlaceableComponent *MagicCircle = CFactory::Instance()->Get<CPlaceableComponent>("Magic circle");
			MagicCircle->SetAngle(MagicCircle->GetAngle() + 1000.0f * CEngine::Instance()->GetDeltaTime());
			CPlaceableComponent *temptext = static_cast<CPlaceableComponent*>(CFactory::Instance()->Get<CGameObject>("Mouse cursor")->Parent);
			temptext->SetAngle(temptext->GetAngle() + 100.0f * CEngine::Instance()->GetDeltaTime());
			CPlaceableComponent *onemore = CFactory::Instance()->Get<CPlaceableComponent>("Magic square");
			onemore->SetAngle(temptext->GetAngle() + 100.0f * CEngine::Instance()->GetDeltaTime());
		}
	}
};

bool CCustomStateHandler::OnInitialize()
{

	CEngine::Instance()->SetDoShowFPS(true);

	CAbstractScene *NewScene = CSceneManager::Instance()->CreateScene();
	CSceneManager::Instance()->SetCurrentScene(NewScene);

	CScriptableComponent *sc = new CScriptableComponent(CFactory::Instance()->Get<CScript>("scriptable-test"));
	CFactory::Instance()->Add(sc, "ScriptableComponent");

	CPlaceableComponent *tempMagicCirclePlacement = CFactory::Instance()->New<CPlaceableComponent>("Magic circle");
	CPlaceableComponent *tempMagicSquarePlacement = CFactory::Instance()->New<CPlaceableComponent>("Magic square");

	tempMagicSquarePlacement->SetPosition(Vector2(50.0f, 50.0f));
	tempMagicCirclePlacement->SetPosition(Vector2(100.0f, 100.0f));




	CRenderableComponent *NewRenderableComponent = CFactory::Instance()->New<CRenderableComponent>();
	NewRenderableComponent->SetModel(CRenderManager::CreateModelCircle(20.0f, MODEL_TYPE_LINES, 4));
	CRenderableComponent *magic_square = CFactory::Instance()->New<CRenderableComponent>();
	magic_square->SetModel(CRenderManager::CreateModelCircle(10.0f, MODEL_TYPE_LINES, 16));

	CRenderableComponent *temptext = CFactory::Instance()->Get<CRenderableComponent>("Mouse cursor");
	CSomeUpdatable *SomeUpdatable = new CSomeUpdatable();

	tempMagicCirclePlacement->Attach(NewRenderableComponent);
	tempMagicSquarePlacement->Attach(magic_square);

	CUpdateManager::Instance()->RootGameObject->Attach(SomeUpdatable);
	CUpdateManager::Instance()->RootGameObject->Attach(sc);
	tempMagicCirclePlacement->Attach(tempMagicSquarePlacement);
	temptext->Parent->Attach(tempMagicCirclePlacement);


	// Lua scripting test
	//CLuaVirtualMachine::Instance()->RunFile(CResourceManager::Instance()->DataPath + "/Scripts/test.lua");
	//CLuaVirtualMachine::Instance()->RunScript(CFactory::Instance()->Get<CScript>("test"));
	//CLuaVirtualMachine::Instance()->RunScript(CFactory::Instance()->Get<CScript>("scriptable-test"));
 
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
 

// NewRenderableComponent->Attach(NewNewRenderableComponent);
// NewNewRenderableComponent->Attach(NewNewNewRenderableComponent);
 //CUpdateManager::Instance()->RootGameObject->Attach(TestObject);
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
