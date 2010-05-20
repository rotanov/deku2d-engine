#include "2de_Engine.h"
#include "Game.h"
#include "2de_MathUtils.h"
//#include "LuaUtils.h"

bool Init()
{	
// 	Vector2 V(1.0f, 1.0f), U(2.0f, 2.0f), W(3.0f, 3.0f);
// 	(V += U) += W;
// 	W  = V / 0.0f;
	//CTestUnit *Unit = new CTestUnit;
	return true;
}

bool Draw()
{	
	Vector2 p0(320.0f, 240.0f), p1(360.0f, 260.0f);
	float r = 50;
	CPrimitiveRender pr;
	pr.grCircleL(p0, r);
	pr.grCircleL(p1, r);

	double d2 = (p1 - p0) * (p1 - p0);
	if (d2 > 4.0 * r * r)
		return true;
	double temp = sqrt(r*r / d2 - 0.25);
	Vector2 MiddlePoint = (p1 + p0) * 0.5;
	pr.grCircleC(MiddlePoint, 2);

	pr.grCircleC(Vector2( MiddlePoint.x + temp * (p1.y - p0.y), MiddlePoint.y - temp * (p1.x - p0.x)), 2);
	pr.grCircleC(Vector2( MiddlePoint.x - temp * (p1.y - p0.y), MiddlePoint.y + temp * (p1.x - p0.x)), 2);
	//unsigned long FPS = 0;
	//CEngine::Instance()->GetState(CEngine::STATE_FPS_COUNT, &FPS);
	//CFontManager::Instance()->Print(0.0f, 0.0f, 0.0f, itos(FPS));
	return true;
}


int	main(int argc, char *argv[])
{
	CEngine* Ninja = CEngine::Instance();
	Ninja->SetState(CEngine::STATE_CONFIG_PATH, "Config/");
	Ninja->SetState(CEngine::STATE_CONFIG_NAME, "Sandbox.xml");
	Ninja->SetState(CEngine::STATE_USER_INIT_FUNC, &Init);
	Ninja->SetState(CEngine::STATE_RENDER_FUNC, &Draw);	
	Ninja->Run();
	return 0;
}
