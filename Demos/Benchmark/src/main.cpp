#include "2de_Engine.h"

CEngine *Engine = CEngine::Instance();

class CRotatingQuad : public CRenderableComponent
{
public:
	CRotatingQuad()
	{
		CEventManager::Instance()->Subscribe("EveryFrame", this);
		SetBlendingMode(BLEND_MODE_OPAQUE);
		SetLayer(-1);
		float ScrWidth = CGLWindow::Instance()->GetWidth();
		float ScrHeight = CGLWindow::Instance()->GetHeight();
		int Width_d2 = Random_Int(5, 10);
		int Height_d2 = Random_Int(5, 10);
		float x = Random_Float(Width_d2, ScrWidth - Width_d2);
		float y = Random_Float(Height_d2, ScrHeight - Height_d2);
		SetPosition(Vector2(x, y));
		SetBox(CBox(Vector2(-Width_d2, -Height_d2), Vector2(Width_d2, Height_d2)));
		SetColor(RGBAf(Random_Float(0.0f, 1.0f), Random_Float(0.0f, 1.0f), Random_Float(0.0f, 1.0f), 1.0f));
		SetModel(CRenderManager::CreateModelBox(GetBox().Width(), GetBox().Height(), MODEL_TYPE_TRIANGLES));
		//SetAngle(Random_Float(0.0f, 360.0f));
	}

	void ProcessEvent(const CEvent &AEvent)
	{
		if (AEvent.GetName() == "EveryFrame")
		{
			SetAngle(GetAngle() + CEngine::Instance()->GetDeltaTime() * 200.0f);
		}

	}
};

class CRectangleTest : public CObject
{
public:
	CText CounterText;
	int RectangleCount;
	CRectangleTest()
	{
		RectangleCount = 0;
		CEventManager::Instance()->Subscribe("KeyPress", this);
		if (CCommandLineArgumentsManager::Instance()->IsOptionExists("rectangles"))
		{
			RectangleCount = stoi(CCommandLineArgumentsManager::Instance()->GetOption("rectangles"));
			for (int i = 0; i < RectangleCount; i++)
				CUpdateManager::Instance()->RootGameObject->Attach(CFactory::Instance()->New<CRotatingQuad>(""));
		}
		else
		{
			RectangleCount = 0;
		}
		CounterText.SetText("Rectangles count: " + itos(RectangleCount));
		CounterText.SetLayer(1);
		CounterText.SetPosition(Vector2(0.0f, 15.0f));
	}

	void ProcessEvent(const CEvent &AEvent)
	{
		if (AEvent.GetName() == "KeyPress" && AEvent.GetData<Uint16>("Sym") == SDLK_UP)
		{
			//for(int i = 0; i < 43000; i++)
			CUpdateManager::Instance()->RootGameObject->Attach(CFactory::Instance()->New<CRotatingQuad>(""));
			//CounterText.SetText("Rectangles count: " + itos(RectangleCount += 43000));
			CounterText.SetText("Rectangles count: " + itos(RectangleCount += 1));
		}
	}
};

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	bool OnInitialize();
	bool OnArgumentsProcessing();
};

bool CCustomStateHandler::OnArgumentsProcessing()
{
	if (!CCommandLineArgumentsManager::Instance()->RegisterOption("rectangles"))
		return false;

	return true;
}

bool CCustomStateHandler::OnInitialize()
{
	CEngine::Instance()->SetDoKeyRepeat(true);
	CFactory::Instance()->New<CRectangleTest>("Rectangle test");
	return true;
}

int main(int argc, char *argv[])
{
	Engine->SetProgramName("Benchmark");
	Engine->SetStateHandler<CCustomStateHandler>();
	Engine->Run(argc, argv);
	return 0;
}



