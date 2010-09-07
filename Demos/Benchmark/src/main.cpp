#include "2de_Engine.h"

CEngine *Engine = CEngine::Instance();

class CRotatingQuad : public CRenderable, CUpdatable
{
public:
	CRotatingQuad()
	{
		float ScrWidth = CGLWindow::Instance()->GetWidth();
		float ScrHeight = CGLWindow::Instance()->GetHeight();
		int Width_d2 = Random_Int(5, 10);
		int Height_d2 = Random_Int(5, 10);
		float x = Random_Float(Width_d2, ScrWidth - Width_d2);
		float y = Random_Float(Height_d2, ScrHeight - Height_d2);
		Position = Vector2(x, y);
		SetBox(CBox(Vector2(-Width_d2, -Height_d2), Vector2(Width_d2, Height_d2)));
		Color = RGBAf(Random_Float(0.0f, 1.0f), Random_Float(0.0f, 1.0f), Random_Float(0.0f, 1.0f), 1.0f);
		SetAngle(Random_Float(0.0f, 360.0f));
	}
	void Render()
	{
		CRenderManager::Instance()->DrawSolidBox(this, GetBox());
	}
	void Update(float dt)
	{
		SetAngle(GetAngle() + dt * 200.0f);
	}
};

class CAbstractTest : public CUpdatable
{
public:
	void Update(float dt)
	{
		
	}
};

class CRectangleTest : public CAbstractTest
{
public:
	CText CounterText;
	int RectangleCount;
	CRectangleTest()
	{
		CEngine::Instance()->AddKeyInputFunction(&CObject::InputHandling, this);
		if (CCommandLineArgumentsManager::Instance()->IsOptionExists("rectangles"))
		{
			RectangleCount = stoi(CCommandLineArgumentsManager::Instance()->GetOption("rectangles"));
			for (int i = 0; i < RectangleCount; i++)
				CFactory::Instance()->New<CRotatingQuad>("");
		}
		else
		{
			RectangleCount = 0;
		}
		CounterText.SetText("Rectangles count: " + itos(RectangleCount));
		CounterText.SetLayer(1);
		CounterText.Position = Vector2(0.0f, 15.0f);
	}
	void Update(float dt)
	{
		if (CEngine::Instance()->keys[SDLK_UP])
		{
			CFactory::Instance()->New<CRotatingQuad>("");
			CounterText.SetText("Rectangles count: " + itos(++RectangleCount));
		}
	}
	bool InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter)
	{
		switch(state)
		{
			case KEY_DOWN:
				switch (key)
				{
				case SDLK_UP:
					
					break;
				}
			break;
		}
		return true;
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
	// do something..
	// i put it here just to show how it works and as placeholder for user's code..
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



