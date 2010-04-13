#include "2de_Engine.h"
#include "2de_GameUtils.h"

CEngine	*Ninja	=	CEngine::Instance();

CLevelMap *Map;
CTileset *Tiles;
int PlayerX = 4;
int PlayerY = 4;

class CInputHandler : public CObject
{
public:
	CInputHandler()
	{
		SetName("Some InputHandler");
		Ninja->AddKeyInputFunction(&CObject::InputHandling, this);
	}
	bool InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter)
	{
		if (state == KEY_PRESSED)
		{
			switch(key)
			{
			case SDLK_LEFT:
				PlayerX--;
				break;
			case SDLK_RIGHT:
				PlayerX++;
				break;
			case SDLK_UP:
				PlayerY++;
				break;
			case SDLK_DOWN:
				PlayerY--;
				break;

			}
		}
		return true;
	}
};

bool Init()
{	
	CInputHandler* IH = new CInputHandler;
	Tiles = CFactory::Instance()->Get<CTileset>("PacManTileset");
	Tiles->CheckLoad();
	Map = new CLevelMap(10, 10, "PacManTileset", "Pacman map");
//	Map.TileSet = Tiles;
	for(int i = 0; i < 10; i++)
		for(int j = 0; j < 10; j++)
			if ((i*j == 0 || i == 9 || j == 9))
				Map->Cells[Map->_Cell(i, j)].index = 1;
			else if (i != 4 || j != 4)
				Map->Cells[Map->_Cell(i, j)].index = rand() % 100 < 25 ? 1 : 0;

	Map->GenCells();
	return true;
}

bool Render()
{
	glLoadIdentity();
	glClearColor(0.3f, 0.4f, 0.3f, 1.0f);
	//glColor3f(1.0f, 1.0f, 1.0f);
	gSetBlendingMode();
	glTranslatef(16.0f, 16.0f, 0.0f);
	glEnable(GL_TEXTURE_2D);
	Tiles->GetTexture()->Bind();
	glBegin(GL_QUADS);
	for(int i = 0; i < 10; i++)
		for(int j = 0; j < 10; j++)
			{
				if ((PlayerX == i && PlayerY == j))
					continue;
				(Tiles->GetCellTC(Map->Cells[Map->_Cell(i, j)].index)[0]).glTexCoord();
				(Vector2(i, j)*64).glVertex();
				Tiles->GetCellTC(Map->Cells[Map->_Cell(i, j)].index)[1].glTexCoord();
				(Vector2(i+1, j)*64).glVertex();
				Tiles->GetCellTC(Map->Cells[Map->_Cell(i, j)].index)[2].glTexCoord();
				(Vector2(i+1, j+1)*64).glVertex();
				Tiles->GetCellTC(Map->Cells[Map->_Cell(i, j)].index)[3].glTexCoord();
				(Vector2(i, j+1)*64).glVertex();
				
			}
	glEnd();

	glBegin(GL_QUADS);
		(Tiles->GetCellTC(2)[0]).glTexCoord();
		(Vector2(PlayerX, PlayerY)*64).glVertex();
		Tiles->GetCellTC(2)[1].glTexCoord();
		(Vector2(PlayerX+1, PlayerY)*64).glVertex();
		Tiles->GetCellTC(2)[2].glTexCoord();
		(Vector2(PlayerX+1, PlayerY+1)*64).glVertex();
		Tiles->GetCellTC(2)[3].glTexCoord();
		(Vector2(PlayerX, PlayerY+1)*64).glVertex();
	glEnd();

	return true;
}


int	main(int argc, char *argv[])
{
	Ninja->SetState(CEngine::STATE_CONFIG_PATH, "Config/");
	Ninja->SetState(CEngine::STATE_CONFIG_NAME, "Pac man.xml");
	Ninja->SetState(CEngine::STATE_USER_INIT_FUNC, &Init);
	Ninja->SetState(CEngine::STATE_RENDER_FUNC, &Render);
	Ninja->Run();
	return 0x00;
}
