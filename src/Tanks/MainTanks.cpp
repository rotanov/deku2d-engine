#include "2de_Engine.h"
//#include "Game.h"
#include "Tanks.h"

CEngine* Ninja = CEngine::Instance();
//////////////////////////////////////////////////////////////////////////
CFont* Font;
int ScreenWidth = 0;
int ScreenHeight = 0;
Vector2 fPosition = V2_Z;
CParticleSystem *FontEffect;
bool begintimeout = false;
float TimeOut = 0, TimeLimit = 2.0f;
RGBAf FadeClr = RGBAf(1.0f, 1.0f, 1.0f ,1.0f);
//////////////////////////////////////////////////////////////////////////
CTankManager* Tanks;

#define SCALE_TITLE 4.0f
#define TITLE_TEXT "Tanks"


//////////////////////////////////////////////////////////////////////////
///!!!///Где-то в цикле обновления танчиков мы не освобождаем память и как результат - неебический memory leak
//////////////////////////////////////////////////////////////////////////

class CTitleScreen : public CRenderObject, public CUpdateObject
{
public:
	CTitleScreen()
	{
		name = "Title screen tanks";
	}
	~CTitleScreen()
	{		
		
	}
	bool Render()
	{
		glLoadIdentity();
		Font->Pos = fPosition;
		Font->scale.In(SCALE_TITLE, SCALE_TITLE);

		FadeClr.glSet();
		FontEffect->Info.sc = FadeClr;
		FontEffect->Render();
		Font->tClr = FadeClr;
		Font->Print(TITLE_TEXT);
		return true;
	}
	bool Update(scalar dt)
	{
		if (begintimeout)
		{
			TimeOut += dt;
			FadeClr -= RGBAf(0.02, 0.02, 0.02, 0.02);
			if (TimeOut >= TimeLimit)
			{
				Ninja->RenderManager.DelObject("Title screen tanks");
				Ninja->UpdateManager.DelObject("Title screen tanks");
				dynamic_cast<CRenderObject*>(GuiManager.GetObject("Root menu item"))->visible = false;
				Tanks = new CTankManager;	
			}
		}
		return true;
	}
};

bool StartGame()
{
	begintimeout = true;
	return true;
}

bool EndGame()
{
	SDLGLExit(1);
	return true;
}
bool Options()
{
	FadeClr = (Random_Int(0,1)==0)?COLOR_P1:COLOR_P2;
	return true;
}

CTitleScreen *TitleScreen;

bool Init()
{	
		Font = Ninja->FontManager->GetFont("Font");
		Ninja->FontManager->SetCurrentFont("Font");
	//////////////////////////////////////////////////////////////////////////
		Ninja->GetState(STATE_SCREEN_WIDTH, &ScreenWidth);
		Ninja->GetState(STATE_SCREEN_HEIGHT, &ScreenHeight);
		fPosition.x = ScreenWidth - Font->GetStringWidth(TITLE_TEXT)*SCALE_TITLE;
		fPosition.y = ScreenHeight - Font->GetStringHeight(TITLE_TEXT)*SCALE_TITLE;
		fPosition *= 0.5f;
	//////////////////////////////////////////////////////////////////////////
		FontEffect = new CParticleSystem;
		FontEffect->name = "Title bubbles";
		FontEffect->Init();
		FontEffect->Info.sc = RGBAf(0.5f, 0.5f, 0.5f, 1.0f);
		FontEffect->Info.ec = RGBAf(0.0f, 0.0f, 0.0f, 0.5f);
		FontEffect->visible = true;	
		Vector2 *pnts2 = new Vector2 [2];
		pnts2[1] = fPosition + Vector2(0.0f, 20.0f);
		pnts2[0] = fPosition + Vector2(Font->GetStringWidth(TITLE_TEXT)*SCALE_TITLE, 20.0f);
		FontEffect->Info.plife = 1.0;
		FontEffect->Info.emission = 10;///*debug 1 */ 3000;
		FontEffect->Info.startsize = 10;
		FontEffect->Info.sizevar = 2.0f;
		FontEffect->Info.p = Vector2(0, 0);
		FontEffect->Texture = Ninja->TextureManager->GetTextureByName("Particle");
		FontEffect->SetGeometry(pnts2, 2);
	//////////////////////////////////////////////////////////////////////////
	GuiManager.visible = false;
	TitleScreen = new CTitleScreen;
	
	CMenuItem *MenuRoot = new CMenuItem;
	MenuRoot->name = "Root menu item";
	MenuRoot->text = "Root";

	CMenuItem *next = new CMenuItem();
	next->name = "menu item Start";
	next->text = "Start";
	next->CallProc = &StartGame;
	next->position = Vector2(ScreenWidth*0.5f - 20,180);
	next->SetParent(MenuRoot);
	MenuRoot->AddObject(next);

	next = new CMenuItem();
	next->name = "menu item Options";
	next->text = "Options";
	next->position = Vector2(ScreenWidth*0.5f - 20,160);
	next->CallProc = NULL;
	next->SetParent(MenuRoot);
	MenuRoot->AddObject(next);

		CMenuItem *options = new CMenuItem();
		options->name = "menu item Game";
		options->text = "Game";
		options->position = Vector2(ScreenWidth*0.5f - 20, 180);
		options->CallProc = &Options;
		options->SetParent(next);
		next->AddObject(options);

		options = new CMenuItem();
		options->name = "menu item Options";
		options->text = "Sound";
		options->position = Vector2(ScreenWidth*0.5f - 20, 160);
		options->CallProc = NULL;
		options->SetParent(next);
		next->AddObject(options);

		options = new CMenuItem();
		options->name = "menu item Options";
		options->text = "Video";
		options->position = Vector2(ScreenWidth*0.5f - 20,140);
		options->CallProc = NULL;
		options->SetParent(next);
		next->AddObject(options);


	next = new CMenuItem();
	next->name = "menu item Exit";
	next->text = "Exit";
	next->position = Vector2(ScreenWidth*0.5f - 20,140);
	next->CallProc = &EndGame;
	next->SetParent(MenuRoot);
	MenuRoot->AddObject(next);

	dynamic_cast<CRenderObject*>(GuiManager.GetObject("Root menu item"))->visible = true;
	GuiManager.SetFocus(MenuRoot);

	CButton Button(CAABB(10,10,100,50), "Satana.!", RGBAf(1.0f, 0.3f, 1.0f, 1.0f), NULL);	// Этот баттон будет удалён в конце инициализации, т.к. автоматическая память.
	new CButton(CAABB(10,10,100,50), "Satana.!", RGBAf(1.0f, 0.3f, 1.0f, 1.0f), NULL);	// A этот будет работать!
	new CEdit();

// 	CTileSet TanksTiles;
// 	TanksTiles.Info.HorNumTiles = 4;
// 	TanksTiles.Info.VerNumTiles = 2;
// 	TanksTiles.Info.TileWidth = TanksTiles.Info.TileHeight = 32;
// 	TanksTiles.Texture = Ninja->TextureManager->GetTextureByName("TanksTiles");
// 	TanksTiles.BBox = new CAABB [8];
// 	TanksTiles.BBox[0] = CAABB(14, 46, 4, 4);
// 	TanksTiles.BBox[1] = CAABB(0, 0, 32, 32);
// 	TanksTiles.BBox[2] = CAABB(0, 0, 32, 32);
// 	TanksTiles.BBox[3] = CAABB(0, 0, 32, 32);
// 	TanksTiles.BBox[4] = CAABB(0, 0, 32, 32);
// 	TanksTiles.BBox[5] = CAABB(0, 0, 32, 32);
// 	TanksTiles.BBox[6] = CAABB(0, 0, 32, 32);
// 	TanksTiles.BBox[7] = CAABB(0, 0, 32, 32);
// 	TanksTiles.filename = "Data/Tilesets/TanksTileset.tls";
// 	TanksTiles.SaveToFile();
	return true;
}


int	main(int argc, char *argv[])
{
	Ninja->SetState(STATE_CONFIG_NAME, "Tanks.xml");
	Ninja->SetState(STATE_USER_INIT_FUNC, &Init);
	Ninja->Run();
	return 0x1;
}




/*

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "testGame.h"
#include "../tnl/tnlLog.h"

#include "../glut/glutInclude.h"

static const char *localBroadcastAddress = "IP:broadcast:28999";
static const char *localHostAddress = "IP:localhost:28999";

void createGameClient(bool pingLocalHost = false)
{
delete clientGame;
delete serverGame;

// create a client, by default pinging the LAN broadcast on port 28999
clientGame = new TestGame(false,TNL::Address(TNL::IPProtocol, TNL::Address::Any, 0),TNL::Address(pingLocalHost ? localHostAddress : localBroadcastAddress));
serverGame = NULL;
}

void createGameServer()
{
delete clientGame;
delete serverGame;

clientGame = NULL;
// create a server, listening on port 28999.
serverGame = new TestGame(true,TNL::Address(TNL::IPProtocol,TNL::Address::Any, 28999),TNL::Address(localBroadcastAddress));
}

void createGameClientServer()
{
delete clientGame;
delete serverGame;

// construct two local games, one client and one server, and have the client game ping the localhost
// loopback address.
clientGame = new TestGame(false,TNL::Address(TNL::IPProtocol, TNL::Address::Any, 0),TNL::Address(localHostAddress));
serverGame = new TestGame(true,TNL::Address(TNL::IPProtocol,TNL::Address::Any, 28999),TNL::Address(localBroadcastAddress));

// If the following line is commented out, the local games will create a normal (network) connection.
// the createLocalConnection call constructs a short-circuit connection that works without
// network access at all.
clientGame->createLocalConnection(serverGame);
}

void onExit()
{
delete clientGame;
delete serverGame;
}


int gWindowWidth = 400;
int gWindowHeight = 400;

void reshape(int nw, int nh)
{
gWindowWidth = nw;
gWindowHeight = nh;
}

void mouse(int button, int state, int x, int y)
{
static int mouseState=0;

if(!state) // mouse up
{
Position p;
p.x = x / TNL::F32(gWindowWidth);
p.y = y / TNL::F32(gWindowHeight);

if(clientGame)
clientGame->moveMyPlayerTo(p);
else if(serverGame)
serverGame->moveMyPlayerTo(p);
}
}


void idle()
{
if(clientGame)
clientGame->tick();
if(serverGame)
serverGame->tick();
glutPostRedisplay();
}

void display()
{
if(clientGame)
clientGame->renderFrame(gWindowWidth, gWindowHeight);
else if(serverGame)
serverGame->renderFrame(gWindowWidth, gWindowHeight);
glFlush();
glutSwapBuffers();
}

void menu(int value)
{
switch(value)
{
case 1:
createGameClient(false);
break;
case 2:
createGameServer();
break;
case 3:
createGameClientServer();
break;
case 4:
createGameClient(true);
break;
}
}

#include <stdio.h>
class StdoutLogConsumer : public TNL::LogConsumer
{
public:
void logString(const char *string)
{
printf("%s\r\n", string);
}
} gStdoutLogConsumer;

int main(int argc, char **argv)
{
TNLLogEnable(LogGhostConnection, true);
createGameClient();
glutInitWindowSize(gWindowWidth, gWindowHeight);
glutInit(&argc, argv);

glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
glutCreateWindow("TNLTest - Right Button for Menu");
glutDisplayFunc(display);
glutReshapeFunc(reshape);
glutMouseFunc(mouse);
glutIdleFunc(idle);
atexit(onExit);

glutCreateMenu(menu);
glutAddMenuEntry("Restart as client", 1);
glutAddMenuEntry("Restart as server", 2);
glutAddMenuEntry("Restart as client/server", 3);
glutAddMenuEntry("Restart as client pinging localhost", 4);
glutAttachMenu( GLUT_RIGHT_BUTTON );

glutMainLoop();
}

*/