#include "Color Lines.h"
using namespace Deku2D;

enum EPossibleDirections	// Constants for filled lines processing
{
	DIRECTION_BOTH_VERTICAL,
	DIRECTION_UP,
	DIRECTION_DOWN,
	DIRECTION_BOTH_HORIZONTAL,
	DIRECTION_LEFT,
	DIRECTION_RIGHT,
	DIRECTION_BOTH_SLASHLIKE,
	DIRECTION_LEFT_UP,
	DIRECTION_RIGHT_DOWN,
	DIRECTION_BACKSLASHLIKE,
	DIRECTION_LEFT_DOWN,
	DIRECTION_RIGHT_UP,
};

//////////////////////////////////////////////////////////////////////////
// CGridGame
CGridGame::CGridGame()
{
	GLWindow->Resize(320, 340);

	Grid = static_cast<CGameObject*>(Factory->CreateByName("GridProto", "LinesGrid"));
	CEngine::Instance()->RootGameObject->Attach(Grid);

	static_cast<CRenderableComponent*>(Grid->GetChild(0)->GetChild(0))->SetColor(RGBAf(.6f, 0.6f, 0.8f, 1.0f));
	static_cast<CPlaceableComponent*>(Grid->GetChild(0))->SetScaling(static_cast<float>(CELL_SIZE));
	GridPos = GLWindow->GetSize() * 0.5f - Vector2(CELLS_COUNT * CELL_SIZE / 2, CELLS_COUNT * CELL_SIZE / 2);
	static_cast<CPlaceableComponent*>(Grid->GetChild(0))->SetPosition(GridPos);

	EventManager->Subscribe("MouseDown", this);
	EventManager->Subscribe("KeyDown", this);
	EventManager->Subscribe("MouseMove", this);
	EventManager->Subscribe("WindowResize", this);

	(Factory->Get<CRenderableComponent>("TitleScreenClassForInst"))->SetColor(RGBAf(1.0f, 1.0f, 1.0f, 0.5f));

	GridMouseHighlight = Factory->New<CRenderableComponent>("GridMouseHighlight");
	GridMouseHighlight->SetModel(CRenderManager::CreateModelBox(1.05f, 1.05f, MODEL_TYPE_TRIANGLES));
	GridMouseHighlightPlacing = Factory->New<CPlaceableComponent>("GridMouseHighlightPlacing");
	Grid->GetChild(0)->Attach(GridMouseHighlightPlacing);
	GridMouseHighlightPlacing->Attach(GridMouseHighlight);
	GridMouseHighlightPlacing;
	GridMouseHighlight->SetColor(RGBAf(1.0f, 1.0f, 1.0f, 0.5f));

	InitializeGame();
}

void CGridGame::InitializeGame()
{
	TurnSourceCell = TurnTargetCell = CCellSelection();
	FreeCellsPool.clear();
	for(unsigned i = 0; i < CELLS_COUNT; i++)
		for(unsigned j = 0; j < CELLS_COUNT; j++)
			SpawnBall(CCellSelection(i, j), std::numeric_limits<unsigned>::max());

	// Sorry, ignoring "no more than 3 same color balls" condition
	for(unsigned i = 0; i < 5; i++)
		SpawnBall(CCellSelection(Random_Int(0, CELLS_COUNT - 1),
			Random_Int(0, CELLS_COUNT - 1)), Random_Int(0, COLORS_COUNT - 1));
}

void CGridGame::ProcessEvent( const CEvent &AEvent )
{
	if (AEvent.GetName() == "WindowResize")
	{
		GridPos = GLWindow->GetSize() * 0.5f - 
			Vector2(CELLS_COUNT * CELL_SIZE / 2, CELLS_COUNT* CELL_SIZE / 2);
		static_cast<CPlaceableComponent*>(Grid->GetChild(0))->SetPosition(GridPos);	
		return;
	}

	if (AEvent.GetName() == "MouseMove")
	{
		SelectCellByMousePosition(CEngine::Instance()->GetMousePosition());
		return;
	}

	if (AEvent.GetName() == "KeyDown")
	{
		InitializeGame();
		return;
	}

	if (AEvent.GetName() != "MouseDown")
		return;

	CCellSelection CellSelection = SelectCellByMousePosition(CEngine::Instance()->GetMousePosition());

	if (FreeCellsPool.empty())
		InitializeGame();

	if (!TurnSourceCell.isValid())
	{
		if (CellSelection.isValid() && Cells[CellSelection.GetX()][CellSelection.GetY()].GetColorIdentifier() != -1) 
			TurnSourceCell = CellSelection;
		return;
	}

	if (!TurnTargetCell.isValid() && CellSelection.isValid())
	{
		if (CellSelection.isValid())
			if (Cells[CellSelection.GetX()][CellSelection.GetY()].GetColorIdentifier() == -1) 
				TurnTargetCell = CellSelection;
			else
			{
				TurnSourceCell = CellSelection;		
				return;
			}
	}
	else
		return;

	if (ProcessBFS(TurnSourceCell.GetX(), TurnSourceCell.GetY(), TurnTargetCell.GetX(), TurnTargetCell.GetY()))
	{
		SpawnBall(TurnTargetCell, Cells[TurnSourceCell.GetX()][TurnSourceCell.GetY()].GetColorIdentifier());
		SpawnBall(TurnSourceCell, std::numeric_limits<unsigned>::max());

		bool win_some_lines = (ProcessFilledLines(TurnTargetCell.GetX(), TurnTargetCell.GetY(),
			Cells[TurnTargetCell.GetX()][TurnTargetCell.GetY()].GetColorIdentifier()));

		TurnSourceCell = CCellSelection();
		TurnTargetCell = CCellSelection();

		if (win_some_lines)
			return;
		
		for(unsigned i = 0; i < BALLS_PER_TURN; i++)
		{
			unsigned PoolSize = FreeCellsPool.size();
			if (PoolSize == 0)
				continue;
			unsigned Index = Random_Int(0, PoolSize - 1);
			SpawnBall(FreeCellsPool[Index]->GetCoords(), Random_Int(0, COLORS_COUNT - 1));
		}
	}
	else
	{
		TurnTargetCell = CCellSelection();
	}
}

CGridGame::CCellSelection CGridGame::SelectCellByMousePosition( const Vector2 &MousePosition )
{
	CCellSelection CellSelection
		(
		(static_cast<int>(MousePosition.x) - static_cast<int>(GridPos.x)) / CELL_SIZE,
		(static_cast<int>(MousePosition.y) - static_cast<int>(GridPos.y)) / CELL_SIZE
		);

	GridMouseHighlight->SetVisibility(CellSelection.isValid());
	if (CellSelection.isValid())
		GridMouseHighlightPlacing->SetPosition
		(
			Vector2
			(
				CellSelection.GetX() * CELL_SIZE + CELL_SIZE / 2 + 1,
				CellSelection.GetY() * CELL_SIZE + CELL_SIZE / 2 + 1
			)
		);

	return CellSelection;
}

// Something is wrong
int CGridGame::CountBallsInLine(int HorOrVer, int x, int y)
{
	switch (HorOrVer)
	{
	case DIRECTION_BOTH_VERTICAL:
		return CountBallsInLine(DIRECTION_UP, x, y + 1) + 1 + CountBallsInLine(DIRECTION_DOWN, x, y - 1);
		break;
	case DIRECTION_DOWN:
		if (y >= 0 && Cells[x][y].GetColorIdentifier() == Cells[x][y + 1].GetColorIdentifier())
			return 1 + CountBallsInLine(DIRECTION_DOWN, x, y - 1);
		break;
	case DIRECTION_UP:
		if (y < CELLS_COUNT && Cells[x][y].GetColorIdentifier() == Cells[x][y - 1].GetColorIdentifier())
			return 1 + CountBallsInLine(DIRECTION_UP, x, y + 1);
		break;

	case DIRECTION_BOTH_HORIZONTAL:
		return CountBallsInLine(DIRECTION_RIGHT, x + 1, y) + 1 + CountBallsInLine(DIRECTION_LEFT, x - 1, y);
		break;
	case DIRECTION_LEFT:
		if (x >= 0 && Cells[x][y].GetColorIdentifier() == Cells[x + 1][y].GetColorIdentifier())
			return 1 + CountBallsInLine(DIRECTION_LEFT, x - 1, y);
		break;
	case DIRECTION_RIGHT:
		if (x < CELLS_COUNT && Cells[x][y].GetColorIdentifier() == Cells[x - 1][y].GetColorIdentifier())
			return 1 + CountBallsInLine(DIRECTION_RIGHT, x + 1, y);
		break;

	case DIRECTION_BOTH_SLASHLIKE:
		return CountBallsInLine(DIRECTION_LEFT_DOWN, x - 1, y - 1) + 1 + CountBallsInLine(DIRECTION_RIGHT_UP, x + 1, y + 1);
		break;
	case DIRECTION_LEFT_DOWN:
		if (x >= 0 && y >= 0 && Cells[x][y].GetColorIdentifier() == Cells[x + 1][y + 1].GetColorIdentifier())
			return 1 + CountBallsInLine(DIRECTION_LEFT_DOWN, x - 1, y - 1);
		break;
	case DIRECTION_RIGHT_UP:
		if (x < CELLS_COUNT && y < CELLS_COUNT && Cells[x][y].GetColorIdentifier() == Cells[x - 1][y - 1].GetColorIdentifier())
			return 1 + CountBallsInLine(DIRECTION_RIGHT_UP, x + 1, y + 1);
		break;

	case DIRECTION_BACKSLASHLIKE:
		return CountBallsInLine(DIRECTION_LEFT_UP, x - 1, y + 1) + 1 + CountBallsInLine(DIRECTION_RIGHT_DOWN, x + 1, y - 1);
		break;
	case DIRECTION_LEFT_UP:
		if (x >= 0 && y < CELLS_COUNT && Cells[x][y].GetColorIdentifier() == Cells[x + 1][y - 1].GetColorIdentifier())
			return 1 + CountBallsInLine(DIRECTION_LEFT_UP, x - 1, y + 1);
		break;
	case DIRECTION_RIGHT_DOWN:
		if (x < CELLS_COUNT && y >= 0 && Cells[x][y].GetColorIdentifier() == Cells[x - 1][y + 1].GetColorIdentifier())
			return 1 + CountBallsInLine(DIRECTION_RIGHT_DOWN, x + 1, y - 1);
		break;
	}
	return 0;
}

void CGridGame::RemoveBalls(int Dir, unsigned x, unsigned y, unsigned color )
{
	if (!CCellSelection(x, y).isValid())
		return;
	if (Cells[x][y].GetColorIdentifier() != color)
		return;	
	
	switch (Dir)
	{
	case DIRECTION_UP:
		RemoveBalls(DIRECTION_UP, x, y + 1, color);
		break;
	case DIRECTION_DOWN:
		RemoveBalls(DIRECTION_DOWN, x, y - 1, color);
		break;
	case DIRECTION_LEFT:
		RemoveBalls(DIRECTION_LEFT, x - 1, y, color);
		break;
	case DIRECTION_RIGHT:
		RemoveBalls(DIRECTION_RIGHT, x + 1, y, color);
		break;
	case DIRECTION_LEFT_UP:
		RemoveBalls(DIRECTION_LEFT_UP, x - 1, y + 1, color);
		break;
	case DIRECTION_LEFT_DOWN:
		RemoveBalls(DIRECTION_LEFT_DOWN, x - 1, y - 1, color);
		break;
	case DIRECTION_RIGHT_DOWN:
		RemoveBalls(DIRECTION_UP, x + 1, y - 1, color);
		break;
	case DIRECTION_RIGHT_UP:
		RemoveBalls(DIRECTION_UP, x + 1, y + 1, color);
		break;
	}

	SpawnBall(CCellSelection(x, y) , std::numeric_limits<unsigned>::max());
}

bool CGridGame::ProcessFilledLines( unsigned x, unsigned y, unsigned color ) /* sounds like I'm going to make something very bad */
{
	bool AtLeastOneLine = false;
	if (CountBallsInLine(DIRECTION_BOTH_VERTICAL, x, y) >= BALLS_IN_COMPLETED_LINE)
	{
		AtLeastOneLine = true;
		RemoveBalls(DIRECTION_UP, x, y + 1, color);
		RemoveBalls(DIRECTION_DOWN, x, y - 1, color);
	}
	if (CountBallsInLine(DIRECTION_BOTH_HORIZONTAL, x, y) >= BALLS_IN_COMPLETED_LINE)
	{
		AtLeastOneLine = true;
		RemoveBalls(DIRECTION_LEFT, x - 1, y, color);
		RemoveBalls(DIRECTION_RIGHT, x + 1, y, color);
	}
	if (CountBallsInLine(DIRECTION_BOTH_SLASHLIKE, x, y) >= BALLS_IN_COMPLETED_LINE)
	{
		AtLeastOneLine = true;
		RemoveBalls(DIRECTION_LEFT_DOWN, x - 1, y - 1, color);
		RemoveBalls(DIRECTION_RIGHT_UP, x + 1, y + 1, color);
	}
	if (CountBallsInLine(DIRECTION_BACKSLASHLIKE, x, y) >= BALLS_IN_COMPLETED_LINE)
	{
		AtLeastOneLine = true;
		RemoveBalls(DIRECTION_LEFT_UP, x - 1, y + 1, color);
		RemoveBalls(DIRECTION_RIGHT_DOWN, x + 1, y - 1, color);
	}
	if (AtLeastOneLine)
		SpawnBall(CCellSelection(x, y) , std::numeric_limits<unsigned>::max());
	return AtLeastOneLine;
}

void CGridGame::SpawnBall(const CCellSelection &DestCell, unsigned ColorIdentifier)
{
	if (!DestCell.isValid())
		return;

	unsigned x = DestCell.GetX();
	unsigned y = DestCell.GetY();
//	assert(Cells[x][y].isClear());
	Cells[x][y].SetupAt(x, y, ColorIdentifier);

	vector<CCell*>::iterator FreeingCell =  std::find(FreeCellsPool.begin(), FreeCellsPool.end(), &Cells[x][y]);
	if (ColorIdentifier != std::numeric_limits<unsigned>::max() && FreeingCell != FreeCellsPool.end())
	{
		std::swap(*FreeingCell, *(FreeCellsPool.end() - 1));
		FreeCellsPool.erase(FreeCellsPool.end() - 1);
	}
	else if (ColorIdentifier == -1)
		FreeCellsPool.push_back(&Cells[x][y]);
}

bool CGridGame::ProcessBFS(int SourceX, int SourceY, int DestinationX, int DestinationY)
{
	queue<pair<int, int> > Queue;
	Queue.push(make_pair(SourceX, SourceY));
	unsigned MapMarks[CELLS_COUNT][CELLS_COUNT];
	static const unsigned DEKU_INFINITY = static_cast<unsigned>(-1);
	memset(MapMarks, DEKU_INFINITY, CELLS_COUNT * CELLS_COUNT * sizeof(int));
	MapMarks[SourceX][SourceY] = 0;

	while (!Queue.empty())
	{
		int x = Queue.front().first;
		int y = Queue.front().second;
		Queue.pop();

		if (x > 0 && Cells[x - 1][y].GetColorIdentifier() == -1 && MapMarks[x - 1][y] > MapMarks[x][y])
		{
			MapMarks[x - 1][y] = MapMarks[x][y] + 1;
			Queue.push(make_pair(x - 1, y));
		}

		if (y > 0 && Cells[x][y - 1].GetColorIdentifier() == -1 && MapMarks[x][y - 1] > MapMarks[x][y])
		{
			MapMarks[x][y - 1] = MapMarks[x][y] + 1;
			Queue.push(make_pair(x, y - 1));
		}

		if (x < CELLS_COUNT - 1 && Cells[x + 1][y].GetColorIdentifier() == -1 && MapMarks[x + 1][y] > MapMarks[x][y])
		{
			MapMarks[x + 1][y] = MapMarks[x][y] + 1;
			Queue.push(make_pair(x + 1, y));
		}

		if (y < CELLS_COUNT - 1 && Cells[x][y + 1].GetColorIdentifier() == -1 && MapMarks[x][y + 1] > MapMarks[x][y])
		{
			MapMarks[x][y + 1] = MapMarks[x][y] + 1;
			Queue.push(make_pair(x, y + 1));
		}
	}

	unsigned DestMark = MapMarks[DestinationX][DestinationY];
	if (DestMark == DEKU_INFINITY)
		return false;
	int x = DestinationX,  y = DestinationY;
	while (DestMark != 0)
	{
		if (x > 0 && MapMarks[x - 1][y] < MapMarks[x][y])
		{
			x--;
			DestMark = MapMarks[x][y];
			SpawnFloorHightlight(x, y, DestMark);
			continue;
		}

		if (y > 0 && MapMarks[x][y - 1] < MapMarks[x][y])
		{
			y--;
			DestMark = MapMarks[x][y];
			SpawnFloorHightlight(x, y, DestMark);
			continue;
		}

		if (x < CELLS_COUNT - 1 && MapMarks[x + 1][y] < MapMarks[x][y])
		{
			x++;
			DestMark = MapMarks[x][y];
			SpawnFloorHightlight(x, y, DestMark);
			continue;
		}

		if (y < CELLS_COUNT - 1 && MapMarks[x][y + 1] < MapMarks[x][y])
		{
			y++;
			DestMark = MapMarks[x][y];
			SpawnFloorHightlight(x, y, DestMark);
			continue;
		}
	}
	return true;
}

void CGridGame::SpawnFloorHightlight( unsigned x, unsigned y, unsigned TTL )
{
	CLiveBox *NewLiveBox = Factory->New<CLiveBox>();
	Grid->GetChild(0)->Attach(static_cast<CGameObject*>(NewLiveBox));
	NewLiveBox->Placing->SetPosition(Vector2(x, y) * CELL_SIZE  + Vector2(16, 16));
	NewLiveBox->TTL = TTL * 0.15f;
}

//////////////////////////////////////////////////////////////////////////
// CCellSelection
CGridGame::CCellSelection::CCellSelection( int Ax, int Ay ) : x(Ax), y(Ay)
{
	Valid = x >= 0 && y >= 0 && x < CELLS_COUNT && y < CELLS_COUNT;
}

CGridGame::CCellSelection::CCellSelection() : Valid(false), x(0), y(0)
{

}

unsigned CGridGame::CCellSelection::GetX() const
{
	return x;
}

unsigned CGridGame::CCellSelection::GetY() const
{
	return y;
}

bool CGridGame::CCellSelection::isValid() const
{
	return Valid;
}

//////////////////////////////////////////////////////////////////////////
// CCell
CGridGame::CCell::CCell( int AColorIdentifier /*= -1*/ ) : ColorIdentifier(AColorIdentifier), VisualPart(NULL)
{

}

bool CGridGame::CCell::isClear()
{
	return VisualPart == NULL && ColorIdentifier == -1;
}

void CGridGame::CCell::MakeVisualPart()
{
	VisualPart = static_cast<CGameObject*>(Factory->CreateByName("BallProto", ""));
	static_cast<CPlaceableComponent*>(VisualPart->GetChild(0))->SetPosition
		(
			Vector2
			(
				Coords.GetX() * CELL_SIZE + CELL_SIZE / 2,
				Coords.GetY() * CELL_SIZE + CELL_SIZE / 2
			)
		);
	static_cast<CRenderableComponent*>(VisualPart->GetChild(0)->GetChild(0))->SetColor(LINES_COLORS[ColorIdentifier]);
}

void CGridGame::CCell::Invalidate()
{
	if (VisualPart != NULL)
		VisualPart->SetDestroyedSubtree(), VisualPart = NULL;
	ColorIdentifier = std::numeric_limits<unsigned>::max();
	Coords = CCellSelection();
}

void CGridGame::CCell::SetupAt( int x, int y, unsigned Color )
{
	Invalidate();
	Coords = CCellSelection(x, y);
	if (!Coords.isValid())
		return;
	ColorIdentifier = Color;
	if (Color != std::numeric_limits<unsigned>::max())
		MakeVisualPart();
}

unsigned CGridGame::CCell::GetColorIdentifier() const
{
	return ColorIdentifier;
}
