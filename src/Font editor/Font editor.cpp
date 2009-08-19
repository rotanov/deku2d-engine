#include "Engine.h"
#include "LuaUtils.h"
#include "OpenglUtils.h"
#include "GameUtils.h"

CEngine* Ninja = CEngine::Instance();
byte					Zoom  = 6;							// Коэффициент масштабирования.
const int				InterfaceOffsetX = 129;				// Magic numbers here, initializing offsets
int						BoundingRectangleIndex = 0;
byte					GridCellSize = 16;



bool Init()
{
	CButtonMini *btn = new CButtonMini(CAABB(20, 20, 100, 50), "1", RGBAf(0.7f, 0.8f, 0.9f, 1.0f), NULL);
	btn = new CButtonMini(CAABB(20, 100, 100, 50), "2", RGBAf(0.7f, 0.8f, 0.9f, 1.0f), NULL);
	btn = new CButtonMini(CAABB(20, 200, 100, 50), "3", RGBAf(0.7f, 0.8f, 0.9f, 1.0f), NULL);
	btn = new CButtonMini(CAABB(20, 300, 100, 50), "4", RGBAf(0.7f, 0.8f, 0.9f, 1.0f), NULL);
	CEditMini *edit = new CEditMini();
	edit->aabb = CAABB(450, 350, 96, 32);
	edit->text = "Edit demo";
	edit->color = btn->color;
	return true;
}

//glColor3f(.4f, .4f, .4f);
//glColor3f(.5f, .5f, .6f);
//glColor3f(0.6f, 0.7f, 0.8f);
//glColor3f(0.9f, 0.8f, 0.2f);



bool Draw()
{
	return true;
}



int	main(int argc, char *argv[])
{
	Ninja->SetState(STATE_CONFIG_NAME, "FontEditor.xml");
	Ninja->SetState(STATE_USER_INIT_FUNC, &Init);
	Ninja->SetState(STATE_RENDER_FUNC, &Draw);	
	Ninja->Run();
	Ninja->FreeInst();
	return 0x01;
}