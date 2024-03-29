#include "2de_Engine.h"
using namespace Deku2D;
CEngine *Engine = CEngine::Instance();

class CRotatingQuad : public CRenderableComponent
{
public:
	CRotatingQuad()
	{
		EventManager->Subscribe("EveryFrame", this);
		SetBlendingMode(BLEND_MODE_OPAQUE);
		float ScrWidth = GLWindow->GetWidth();
		float ScrHeight = GLWindow->GetHeight();
		int Width_d2 = Random_Int(5, 10);
		int Height_d2 = Random_Int(5, 10);
		float x = Random_Float(Width_d2, ScrWidth - Width_d2);
		float y = Random_Float(Height_d2, ScrHeight - Height_d2);
		//SetBox(CBox(Vector2(-Width_d2, -Height_d2), Vector2(Width_d2, Height_d2)));
		CBox b(Vector2(-Width_d2, -Height_d2), Vector2(Width_d2, Height_d2));
		SetColor(RGBAf(Random_Float(0.0f, 1.0f), Random_Float(0.0f, 1.0f), Random_Float(0.0f, 1.0f), 1.0f));
		SetModel(CRenderManager::CreateModelBox(b.Width(), b.Height(), MODEL_TYPE_TRIANGLES));
		Place = Factory->New<CPlaceableComponent>("");
		Place->SetPosition(Vector2(x, y));
		Place->SetLayer(-1);
		Place->Attach(this);
		CEngine::Instance()->RootGameObject->Attach(Place);
		//SetAngle(Random_Float(0.0f, 360.0f));
	}

	void ProcessEvent(const CEvent &AEvent)
	{
		if (AEvent.GetName() == "EveryFrame")
		{
			Place->SetAngle(Place->GetAngle() + CEngine::Instance()->GetDeltaTime() * 200.0f);
		}

	}
private:
	CPlaceableComponent *Place;
};

class CRectangleTest : public CObject
{
public:
	CText *CounterText;
	int RectangleCount;
	CRectangleTest()
	{
		CounterText = Factory->New<CText>();
		RectangleCount = 0;
		EventManager->Subscribe("KeyPress", this);
		if (CommandLineArgumentsManager->IsOptionExists("rectangles"))
		{
			RectangleCount = Deku2D::stoi(CommandLineArgumentsManager->GetOption("rectangles"));
			for (int i = 0; i < RectangleCount; i++)
				Factory->New<CRotatingQuad>("");
				//CEngine::Instance()->RootGameObject->Attach(Factory->New<CRotatingQuad>(""));
		}
		else
		{
			RectangleCount = 0;
		}
		CPlaceableComponent *TextPlace = Factory->New<CPlaceableComponent>("TextPlace");
		TextPlace->Attach(CounterText);
		CounterText->SetText("Rectangles count: " + itos(RectangleCount));
		TextPlace->SetLayer(1);
		TextPlace->SetPosition(Vector2(0.0f, 15.0f));
		CEngine::Instance()->RootGameObject->Attach(TextPlace);
	}

	void ProcessEvent(const CEvent &AEvent)
	{
		if (AEvent.GetName() == "KeyPress" && AEvent.GetData<Uint16>("Sym") == SDLK_UP)
		{
			//for(int i = 0; i < 43000; i++)
			Factory->New<CRotatingQuad>("");
			//CounterText.SetText("Rectangles count: " + itos(RectangleCount += 43000));
			CounterText->SetText("Rectangles count: " + itos(RectangleCount += 1));
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
	if (!CommandLineArgumentsManager->RegisterOption("rectangles"))
		return false;

	return true;
}

bool CCustomStateHandler::OnInitialize()
{
	CEngine::Instance()->SetDoKeyRepeat(true);
	Factory->New<CRectangleTest>("Rectangle test");
	return true;
}

int main(int argc, char *argv[])
{
	Engine->SetProgramName("Benchmark");
	Engine->SetStateHandler<CCustomStateHandler>();
	Engine->Run(argc, argv);
	return 0;
}



