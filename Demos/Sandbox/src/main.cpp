#include "2de_Engine.h"
#include "Game.h"
//#include "LuaUtils.h"

CEngine* Ninja = CEngine::Instance();

bool Init()
{	
	//CTestUnit *Unit = new CTestUnit;
	return true;
}

bool Draw()
{	
	unsigned long FPS = 0;
	CEngine::Instance()->GetState(STATE_FPS_COUNT, &FPS);
	CFontManager::Instance()->Print(0, 0, 0, itos(FPS));

	CPrimitiveRender Pr;

	Vector2 u0(-2.0f, +0.0f);
	Vector2 u1(+4.0f, +3.0f);
	Vector2 v0(-1.0f, -3.0f);
	Vector2 v1(+1.0f, 8.0f);
	Vector2 Result;

	glTranslatef(320.0f, 240.0f, 0.0f);
	glScaled(10.0f, 10.0f, 1.0f);

	Pr.psize = 0.1f;
	
	Pr.grSegment(u0, u1);
	Pr.grSegment(v0, v1);

	if (IntersectSegments(u0, u1, v0, v1, Result))
	{
		glEnable(GL_POINT);
		glEnable(GL_POINT_SIZE);
		glPointSize(8.0f);
		COLOR_RED.glSet();
		glBegin(GL_POINTS);
			Result.glVertex();
		glEnd();
	}

	float SignTest = -8.0f;

	float SignResult = Sign(SignTest);
	SignResult = Abs(SignTest);

	float dist = DistanceToLine(u0, u1, v1);
	Vector2 Norm = Vector2((u1 - u0).y, -(u1 - u0).x).Normalized();
	Vector2 Point = v1 + Norm * dist;
	Pr.grSegment(Point, v1);

	return true;
}


int	main(int argc, char *argv[])
{
	Ninja->SetState(STATE_CONFIG_PATH, "Config/");
	Ninja->SetState(STATE_CONFIG_NAME, "Sandbox.xml");
	Ninja->SetState(STATE_USER_INIT_FUNC, &Init);
	Ninja->SetState(STATE_RENDER_FUNC, &Draw);	
	Ninja->Run();
	return 0x1;
}
