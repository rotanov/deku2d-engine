#include "2de_Engine.h"

#define ENGINE CEngine::Instance()
#define RENDER CRenderManager::Instance()
CEngine* Ninja = ENGINE;


class CCustomStateHandler : public CAbstractStateHandler
{
public:
	bool OnInitialize();
	void OnFinalize();
};

bool CCustomStateHandler::OnInitialize()
{
	CEngine::Instance()->SetDoShowFPS(true);

	//CAbstractScene *NewScene = CSceneManager::Instance()->CreateScene();
	//CSceneManager::Instance()->SetCurrentScene(NewScene);

	CLuaVirtualMachine::Instance()->RunScript(CFactory::Instance()->Get<CScript>(CConfig::Instance()->Section("Data")["InitScript"]));

	// TODO: get rid of these lines when CModel will be real resource
	CRenderableComponent *MagicSquare = CFactory::Instance()->Get<CRenderableComponent>("Magic square renderable");
	if (MagicSquare)
		MagicSquare->SetModel(CRenderManager::CreateModelCircle(20.0f, MODEL_TYPE_LINES, 4));

	CRenderableComponent *MagicCircle = CFactory::Instance()->Get<CRenderableComponent>("Magic circle renderable");
	if (MagicCircle)
		MagicCircle->SetModel(CRenderManager::CreateModelCircle(10.0f, MODEL_TYPE_LINES, 16));


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
